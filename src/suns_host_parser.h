
#include <stdlib.h>
#include <stdio.h>

#include "ezxml/ezxml.h"
#include "trx/list.h"
#include "suns_model.h"

typedef struct suns_attr_map {
    char *name;
    char **value;
} suns_attr_map_t;


int suns_host_parse_logger_xml(FILE *stream,
                               list_t *devices,
                               suns_host_result_t *result);
int suns_host_parse_sunspec_data(ezxml_t sunspec_data,
				 list_t *devices,
				 suns_host_result_t *result);
int suns_host_parse_model(ezxml_t m,
                          suns_device_t *device,
                          suns_host_dr_fail_t *dr_fail);
int suns_host_parse_datapoint(ezxml_t p,
                              suns_dataset_t *data,
                              suns_host_dr_fail_t *dr_fail);
int suns_host_parse_device(ezxml_t d, list_t *devices,
			   suns_host_dr_fail_t *error);
int suns_parse_xml_attr(ezxml_t x, suns_attr_map_t *map);
