#! /usr/bin/env python

# Copyright (c) 2012, Matt Bartlett <mattdbartlett@yahoo.com>
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#
#     * Neither the name of Matt Bartlett nor the names of its contributors
#       may be used to endorse or promote products derived from this software
#       without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
# Matt Bartlett BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
# USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
# OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.


from optparse import OptionParser
import xml.dom.minidom

#Types
TYPE_STRING = 'string'
TYPE_PAD = 'pad'
TYPE_SUNSSF = 'sunssf'
TYPE_ACC16 = 'acc16'
TYPE_UINT16 = 'uint16'
TYPE_INT16 = 'int16'
TYPE_ACC32 = 'acc32'
TYPE_UINT32 = 'uint32'
TYPE_INT32 = 'int32'
TYPE_BITFIELD32 = 'bitfield32'
TYPE_FLOAT32 = 'float32'

def type_string(point_type, point_len):
    if point_type == TYPE_STRING:
        return "string.{0}".format(int(point_len)*2)
    elif point_type == TYPE_ACC32:
        return TYPE_UINT32
    elif point_type == TYPE_ACC16:
        return TYPE_UINT16
    elif point_type == TYPE_PAD:
        return TYPE_UINT16
    else:
        return point_type

def default_type_value(point_type):
    if point_type == TYPE_STRING:
        return "\"\"";
    elif point_type == TYPE_PAD:
        return "0";
    elif point_type == TYPE_SUNSSF:
        return "0";
    elif point_type == TYPE_ACC16:
        return "0";
    elif point_type == TYPE_UINT16:
        return "0";
    elif point_type == TYPE_INT16:
        return "0";
    elif point_type == TYPE_ACC32:
        return "0";
    elif point_type == TYPE_UINT32:
        return "0";
    elif point_type == TYPE_INT32:
        return "0";
    elif point_type == TYPE_BITFIELD32:
        return "0";
    elif point_type == TYPE_FLOAT32:
        return "0.0";

def get_element_text(dom_element):
    """
        Get the content of the element from it's text node (if one exists)
    """
    return " ".join(t.nodeValue for t in dom_element.childNodes if t.nodeType == t.TEXT_NODE)

class ModelBlock:
    def __init__(self, block_len, block_type="norm"):
        self.points = {}
        self.block_len  = block_len
        self.block_type = block_type

    def __str__(self):
        msg = "  ModelBlock (len={0}, type={1}) {{\n".format(self.block_len, self.block_type)
        for name,point in self.points.iteritems():
            msg += "    {0} {{\n".format(point["id"])
            for key,val in point.iteritems():
                msg += "       {0}: {1}\n".format(key, val)
            msg += "    }\n"
        msg += "  }\n"
        return msg

    def add_point(self, point_name, offset, point_type, mandatory=False, length=0):
        self.points[int(offset)] = {'id':point_name,'offset':offset, 'point_type':point_type, 'mandatory':mandatory, "length":length}

    def to_model(self, offset=0):
        msg =  "  # block type={0}\n".format(self.block_type)
        msg += "  # block length={0}\n".format(self.block_len)

        #sort by offset
        sorted_offsets = sorted(self.points.keys())

        for key in sorted_offsets:
            point_type = self.points[key]['point_type'];
            length = self.points[key]['length'];
            msg += "  {type}: {value} #{name}\n".format(type=type_string(point_type, length), 
                    value=default_type_value(point_type), name=self.points[key]['id'])

        return msg, offset+int(self.block_len)

class Model:
    def __init__(self, filename):
        self.blocks = []
        self.map_number = 0
        self.strings = {}

        #read the device
        xmldoc = xml.dom.minidom.parse(filename)
        models = xmldoc.getElementsByTagName("model")

        #there should only be one model
        self.map_number = models[0].getAttribute("id")

        #Get the strings first
        strings = xmldoc.getElementsByTagName("strings")
        self.strings_locale = strings[0].getAttribute("locale")
        if self.strings_locale is None or self.strings_locale == "":
            self.strings_locale = "en"

        model_strings = strings[0].getElementsByTagName("model")
        self.label = get_element_text(model_strings[0].getElementsByTagName("label")[0])
        self.description = get_element_text(model_strings[0].getElementsByTagName("description")[0])
        self.notes = get_element_text(model_strings[0].getElementsByTagName("notes")[0])

        self.point_strings = {}
        point_strings = strings[0].getElementsByTagName("point")

        for point_string in point_strings:
            name = point_string.getAttribute("id");
            self.point_strings[name] = {}
            for child_node in point_string.childNodes:
                if child_node.nodeType == xml.dom.Node.ELEMENT_NODE:
                    self.point_strings[name][child_node.tagName] = get_element_text(child_node)


        #now read each block (1-2)
        blocks = models[0].getElementsByTagName("block")

        for block in blocks:
            block_len = block.getAttribute("len")
            block_type = block.getAttribute("type")
            if block_type is None or block_type == "":
                block_type = "norm"

            model_block = ModelBlock(block_len, block_type)

            points = block.getElementsByTagName("point")

            for point in points:
                point_name = point.getAttribute("id")
                point_offset = point.getAttribute("offset")
                point_type = point.getAttribute("type")
                point_len = 0
                if point_type == TYPE_STRING:
                    point_len = point.getAttribute("len")

                point_mandatory = point.getAttribute("mandatory")
                if point_mandatory is None:
                    point_mandatory = False

                model_block.add_point(point_name=point_name, offset=point_offset, 
                        point_type=point_type, mandatory=point_mandatory, length=point_len)

            self.blocks.append(model_block)

    def __str__(self):
        msg = "SunSpec map {0} {{\n".format(self.map_number)
        msg += "  label: {0}\n".format(self.label)
        msg += "  description: {0}\n".format(self.description)
        msg += "  notes: {0}\n".format(self.notes)
        for block in self.blocks:
            if block is not None:
                msg += str(block)
        msg += "}\n"
        return msg

    def to_model(self, model_name="model_test"):
        """
            Convert our internal data to the .model format for the sunspec tool
        """
        model_len = 0
       
        block_msgs = []
        for block in self.blocks:
            if block is not None:
                msg, model_len = block.to_model(model_len)
                block_msgs.append(msg)

        model = "data {0} {{\n".format(model_name)
        model += "  #{0}\n".format(self.label)
        model += "  #{0}\n".format(self.description)
        model += "  #{0}\n".format(self.notes)
        model += "  {0}\n".format(self.map_number)
        model += "  {0}\n".format(model_len)

        #insert block data here
        for block_msg in block_msgs:
            model += block_msg

        model += "}\n"
        return model



def main():
    usage = "Usage: %prog [options]"
    description = ("Convert a sunspec map in SMDX format to a base model file")

    parser = OptionParser(usage=usage, description=description)
    parser.add_option("-s", "--smdx",
                      dest="smdx_file",
                      type="string",
                      action="store",
                      help="The location of the smdx file")
    parser.add_option("-m", "--model",
                      dest="model_file",
                      type="string",
                      action="store",
                      default="output.model",
                      help="The name of the model file to write")
    (options, args) = parser.parse_args()

    if (not hasattr(options, "smdx_file") or options.smdx_file is None):
        print "Must specify an smdx file to read"
    else:
        #read the xmdx file
        model = Model(options.smdx_file);
        out_file = open(options.model_file, "w+")
        out_file.write(model.to_model())
        out_file.close()



if __name__ == "__main__":
        main()
