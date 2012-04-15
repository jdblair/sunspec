/* -*- tab-width: 4; indent-tabs-mode: nil -*- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* #include <sqlite3.h> */

#include "suns_model.h"
#include "suns_parser.h"
#include "suns_host.h"
#include "suns_host_parser.h"
#include "suns_output.h"

#include "ezxml/ezxml.h"
#include "trx/macros.h"
#include "trx/date.h"


/*
 * Parse the logger upload xml format and load it into a list of
 * suns_device_t *
 *
 * \param *stream    The FILE* to load the xml from.
 * \param *devices   The list of devices parsed from the xml stream.
 * \param *result    Host result structure.
 *
 * \return 0 on success, negative on error.
 */
int suns_host_parse_logger_xml(FILE *stream,
                               list_t *devices,
                               suns_host_result_t *result)
{
    assert(stream);
    assert(devices);

    int rc = 0;
    ezxml_t data;

    data = ezxml_parse_fp(stream);
    if (data == NULL ||
        data->name == NULL) {
        result->status = STATUS_FAILURE;
        result->error.code = CODE_INVALID_MESSAGE;
        suns_host_err(&(result->error),
                      "malformed xml: %s",
                      ezxml_error(data));
        return -1;
    }

    /* we should be at a sunSpecData node */
    if (strcmp(data->name, "sunSpecData") == 0) {
        rc = suns_host_parse_sunspec_data(data, devices, result);
        if (rc < 0) {
            return rc;
        }
    } else {
        result->status = STATUS_FAILURE;
        result->error.code = CODE_INVALID_MESSAGE;
        debug("found element %s instead of sunSpecData", data->name);
        suns_host_err(&(result->error), 
                      "no sunSpecData element in xml");
        return -1;
        
    }

    return rc;
}


/*
 * Parse a SunSpecData block.
 *
 * \param sunspec_data   The ezxml block containing the SunSpecData
 * \param *devices       The list_t that will be populated with suns_device_t.
 * \param *error         Error message, if there is a parsing error.
 *                       Must free if not NULL.
 * \return               0 on success, negative on error.
 */
int suns_host_parse_sunspec_data(ezxml_t sunspec_data,
                                 list_t *devices,
                                 suns_host_result_t *result)
{
    int rc = 0;
    const char *v;
    ezxml_t d;

    /* start out setting result to success */
    result->status = STATUS_SUCCESS;

    v = ezxml_attr(sunspec_data, "v");
    if (v == NULL) {
        result->status = STATUS_FAILURE;
        result->error.code = CODE_INVALID_MESSAGE;
        suns_host_err(&(result->error),
                                    "No v (version) field in sunSpecData element");
        return -1;
    }
    
    int device_rc = 0;
    suns_host_dr_fail_t *dr_fail;
    for (d = ezxml_child(sunspec_data, "d"); d; d = d->next) {
        dr_fail = suns_host_dr_fail_new();
        device_rc = suns_host_parse_device(d, devices, dr_fail);
        if (device_rc < 0) {
            list_node_add(result->dr_fails, list_node_new(dr_fail));
            result->status = STATUS_DR_FAILURE;
            /* preserve negative exit code for caller */
            rc = device_rc;
            /* don't return - attempt to parse additional devices */
        } else {
            /* discard unused suns_host_dr_fail_t */
            suns_host_dr_fail_free(dr_fail);
        }
    }

    return rc;
}


int suns_host_parse_device(ezxml_t d,
                           list_t *devices,
                           suns_host_dr_fail_t *dr_fail)
{
    int rc = 0;
    suns_device_t *device = suns_device_new();
    char *t = NULL;

    /* this is the list of all possible device attributes */
    suns_attr_map_t device_attr[] = {
        { "cid",   &(device->cid) },            /* O */
        { "id",    &(device->id) },             /* O */
        { "iface", &(device->iface) },          /* O */
        { "lid",   &(device->lid) },            /* C */
        { "man",   &(device->manufacturer) },   /* M */
        { "mod",   &(device->model) },          /* M */
        { "ns",    &(device->ns) },             /* C */
        { "sn",    &(device->serial_number) },  /* M */
        { "t",     &t },                      /* M */
        { NULL,    NULL },
    };

    /* this is the subset that are mandatory */
    suns_attr_map_t mandatory_attr[] = {
        { "man",   &(device->manufacturer) },   /* M */
        { "mod",   &(device->model) },          /* M */
        { "sn",    &(device->serial_number) },  /* M */
        { "t",     &t },                      /* M */
        { NULL,    NULL },
    };

    suns_parse_xml_attr(d, device_attr);

    /* preset dr_fail with the device attributes in case we need it */
    dr_fail->man = device->manufacturer;
    dr_fail->mod = device->model;
    dr_fail->sn = device->serial_number;
    dr_fail->t = t;
    if (device->id)
        dr_fail->id = device->id;

    /* check that we have all the mandatory attributes */
    int i;
    for (i = 0; mandatory_attr[i].name != NULL; i++) {
        if (*(mandatory_attr[i].value) == NULL) {
            dr_fail->error.code = CODE_INVALID_MESSAGE;
            suns_host_err(&(dr_fail->error),
                          "Missing required attribute: "
                          "\"%s\".",
                          mandatory_attr[i].name);
            return -1;
        }
    }

    /* parse the timestamp */
    if (date_parse_rfc3339_to_unixtime_z(t,
                                         &(device->unixtime),
                                         &(device->usec)) < 0) {
        dr_fail->error.code = CODE_INVALID_MESSAGE;
        suns_host_err(&(dr_fail->error),
                                    "invalid rfc3339 timestamp string",
                                    mandatory_attr[i].name);
        return -1;
    }

    /* parse models into suns_dataset_t */
    ezxml_t m;

    for (m = ezxml_child(d, "m"); m; m = m->next) {
        rc = suns_host_parse_model(m, device, dr_fail);
        debug_i(rc);
        if (rc < 0)
            return rc;
    }

    list_node_add(devices, list_node_new(device));

    return rc;
}


int suns_host_parse_model(ezxml_t m,
                          suns_device_t *device,
                          suns_host_dr_fail_t *dr_fail)
{
    int rc = 0;
    suns_dataset_t *data;
    char *id;
    int int_did;
    suns_parser_state_t *sps = suns_get_parser_state();
    ezxml_t p;
    char *x;
    
    data = suns_dataset_new();

    suns_attr_map_t model_attr[] = {
        { "id",   &id },          /* needs to be parsed to an int */
        { "ns",   &(data->ns) },
        { "x",    &x },
        { NULL, NULL },
    };

    suns_parse_xml_attr(m, model_attr);

    if (id == NULL) {
        dr_fail->status = STATUS_FAILURE;
        dr_fail->error.code = CODE_INVALID_MESSAGE;
        suns_host_err(&(dr_fail->error),
                      "Missing required attribute \"id\" "
                      "in m element");
        return -1;
    }

    if (sscanf(id, "%d", &int_did) != 1) {
        dr_fail->status = STATUS_FAILURE;
        dr_fail->error.code = CODE_INVALID_MESSAGE;
        suns_host_err(&(dr_fail->error),
                      "Cannot parse \"id\" attribute value:"
                      "\"%s\"", id);
        return -1;
    }

    /* search for the model */
    data->did = suns_find_did(sps->did_list, int_did);

    if (data->did == NULL) {
        dr_fail->status = STATUS_FAILURE;
        dr_fail->error.code = CODE_UNKNOWN_DEVICE;
        suns_host_err(&(dr_fail->error),
                      "Unknown device id in m element: %d",
                      int_did);
        return -1;
    }

    /* parse index (x) if present */
    if (x != NULL) {
        if (sscanf(x, "%d", &(data->index)) != 1) {
            dr_fail->status = STATUS_FAILURE;
            dr_fail->error.code = CODE_INVALID_MESSAGE;
            suns_host_err(&(dr_fail->error),
                          "can't parse x attribute '%s'", x);
            return -1;
        }            
    }

    for (p = ezxml_child(m, "p"); p; p = p->next) {
        rc = suns_host_parse_datapoint(p, data, dr_fail);
        if (rc < 0)
            return rc;
    }

    list_node_add(device->datasets, list_node_new(data));
    
    return rc;
}


int suns_host_parse_datapoint(ezxml_t p,
                              suns_dataset_t *data,
                              suns_host_dr_fail_t *dr_fail)
{
    suns_model_t *model;
    suns_value_t *v = suns_value_new();
    suns_dp_t *dp;
    char *sf = NULL;
    char *value = NULL;
    char *timestamp = NULL;
    char *x = NULL;

    model = data->did->model;
    
    suns_attr_map_t point_attr[] = {
        { "d",    &(v->description) },
        { "id",   &(v->name) },
        { "sf",   &sf },
        { "t",    &timestamp },
        { "u",    &(v->units) },
        { "x",    &x },
        { NULL,   NULL },
    };

    suns_parse_xml_attr(p, point_attr);

    /* look up the datapoint name in the model */
    dp = suns_search_model_for_dp_by_name(model, v->name);

    if (dp == NULL) {
        dr_fail->status = STATUS_FAILURE;
        dr_fail->error.code = CODE_INVALID_MESSAGE;
        suns_host_err(&(dr_fail->error),
                                    "unrecognized datapoint name: %s",
                                    v->name);
        return -1;
    }

    /* parse timestamp, if present */
    if (timestamp) {
        struct tm tm;
        if (date_parse_rfc3339_tm(timestamp, &tm, &(v->usec)) < 0) {
            dr_fail->status = STATUS_FAILURE;
            dr_fail->error.code = CODE_INVALID_MESSAGE;
            suns_host_err(&(dr_fail->error),
                                        "can't parse timestamp '%s' "
                                        "in datapoint '%s'",
                                        timestamp, v->name);
            return -1;
        }
    }

    /* parse value */
    value = ezxml_txt(p);
    debug("value = %s", value);
    debug("type = %s", suns_type_string(dp->type_pair->type));
    if (value == NULL) {
            dr_fail->status = STATUS_FAILURE;
            dr_fail->error.code = CODE_INVALID_MESSAGE;
            suns_host_err(&(dr_fail->error),
                                        "no value in datapoint '%s'",
                                        v->name);
            return -1;
    }
    if (suns_string_to_value(value, v, dp->type_pair) < 0) {
        dr_fail->status = STATUS_FAILURE;
        dr_fail->error.code = CODE_INVALID_MESSAGE;
        suns_host_err(&(dr_fail->error),
                      "cannot parse value \"%s\" in point %s",
                      value, v->name);
        return -1;
    }

    /* parse scale factor */
    if (sf != NULL) {
#if 0
        if (v->tp.sf != 0) {
            /* this implies that suns_string_to_value() parsed a
               decimal floating point value
               
               this is an error b/c there shouldn't be both a decimal
               floating point in the value AND a scale factor present */
            dr_fail->status = STATUS_FAILURE;
            dr_fail->error.code = CODE_INVALID_MESSAGE;
            suns_host_err(&(dr_fail->error),
                          "point %s contains a decimal value "
                          "'%s' and a sf attribute",
                          v->name, value);
            return -1;
        }
#endif
        
        if (sscanf(sf, "%d", &(v->tp.sf)) != 1) {
            dr_fail->status = STATUS_FAILURE;
            dr_fail->error.code = CODE_INVALID_MESSAGE;
            suns_host_err(&(dr_fail->error),
                          "can't parse sf attribute '%s'", sf);
            return -1;
        }
    }

    /* parse index (x) if present */
    if (x != NULL) {
        if (sscanf(x, "%d", &(v->index)) != 1) {
            dr_fail->status = STATUS_FAILURE;
            dr_fail->error.code = CODE_INVALID_MESSAGE;
            suns_host_err(&(dr_fail->error),
                          "can't parse x attribute '%s'", x);
            return -1;
        }            
    }

    /* add the datapoint to the dataset */
    list_node_add(data->values, list_node_new(v));

    return 0;
}
    
    

/*
 * convenience function used to map xml attributes to a list of
 * char pointers.
 *
 * \param x    The ezxml block to read attributes from.
 * \param map  The map of names to char *
 * \return     The number of mapped attributes
 *
 */
int suns_parse_xml_attr(ezxml_t x, suns_attr_map_t *map)
{
    int i;
    int total = 0;

    /* slot the attributes into the map */
    for (i = 0; map[i].name != NULL; i++) {
        *(map[i].value) = (char *) ezxml_attr(x, map[i].name);
        if (*(map[i].value)) {
            total++;
            debug("%s = %s", map[i].name, *(map[i].value));
        }
    }

    return total;
}

