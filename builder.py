#!/bin/python3

"""
Copyright 2017 Samuel Cowen <samuel.cowen@camelsoftware.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
"""


import sys
import json
import shutil
from pprint import pprint


def is_an_enum(name, data):
    for i in data["enums"]:
        
        if(i["title"] == name):
            print(i["title"] + " " + name)
            print("returning true")
            return True
    return False


def build_docs(data):
    file = open("c3/index.html", "w")
    file.write("""
<html class="ui-page-theme-a">
    <head>
        <link rel="stylesheet" href="style.css" />
        <title>""")
    file.write(data["project"]["title"])
    file.write("""</title>
    </head>
    <body>
        <div class="ui-bar-a">
    <h1>""")
    file.write(data["project"]["title"])
    file.write("</h1><p>")
    file.write(data["project"]["description"])
    file.write("</p><p>Version: ")
    file.write(data["project"]["version"])
    file.write("</p></div>")


    file.write("<div class=\"ui-panel-page-container-a\"><h1>Index</h1><div class=\"ui-body-a\"><h2>Enumerations</h2><ul>")
    for i in data["enums"]:
        file.write("<li><a href=\"#enum_" + i["title"] + "\">" + i["title"] + "</a></li>")
    
    file.write("</ul><br><h2>Packets</h2><ul>")
    for i in data["packets"]:
        file.write("<li><a href=\"#packet_" + i["title"] + "\">" + i["title"] + "</a></li>")

    file.write("</ul></div></div>")

    file.write("<h1>Enumerations</h1>")
    for i in data["enums"]:
        file.write("<h3 id=\"enum_" + i["title"] + "\">" + i["title"] + "</h3>")
        file.write("<p>" + i["description"] + "</p>")
        for f in i["fields"]:
            file.write("<span>" + f + "</span><br>")

    file.write("<h1>Packets</h1>")
    for i in data["packets"]:
        file.write("<h3 id=\"packet_" + i["title"] + "\">" + i["title"] + "</h3>")
        file.write("<span>" + i["description"] + "</span><br><span>")
        if(i["owner"] == "both"):
            file.write("Received by both local and remote.")
        if(i["owner"] == "remote"):
            file.write("Received by local.")
        if(i["owner"] == "local"):
            file.write("Received by remote.")
        file.write("</span><table><tr><th>Name</th><th>Type</th><th>Description</th></tr>")
        for j in i["fields"]:
            file.write("<tr><td>" + j["name"] + "</td><td>" + j["type"] + "</td><td>")
            if "description" in j:
                file.write(j["description"])
            file.write("</td></tr>")

        file.write("</table>")



    file.write("""    </body>
</html>""")
    file.close()

def build_enums_header(data):
    file = open("c3/c3_enums.h", "w")
    file.write("""
#ifndef C3_ENUMS_H
#define C3_ENUMS_H

#include <stdint.h>
namespace ccc {

""")
    for i in data["enums"]:
        file.write("enum " + i["title"] + " : uint8\n{\n\t")
        for f in i["fields"]:
            file.write(i["title"] + "_" + f + ",\n\t")
        file.write(i["title"] + "_NULL\n};\n\n")

    file.write("enum PACKETS : uint8\n{\n\t")
    file.write("ACK_PACK,\n\t")
    for i in data["packets"]:
        file.write(i["title"].upper() + "_PACK,\n\t")
    file.write("NULL_PACK\n};\n\n")
    file.write("}\n\n#endif\n")


def build_packets_header(data):
    file = open("c3/c3_packets.h", "w")
    file.write("""
#ifndef C3_PULLS_IN_PACKETS
#error Do not include c3_packets.h. Only include "c3.h"
#endif

""")
    for i in data["packets"]:
        file.write("class " + i["title"].lower() +
                   "_pack : public base_pack\n")
        file.write("{\npublic:\n\t")
        file.write(i["title"].lower() +
                   "_pack(uint8 sync) : base_pack(sync) { }\n\t")
        file.write("static constexpr uint16 id = " +
                   i["title"].upper() + "_PACK;\n\n\t")

        for v in i["fields"]:
            file.write(v["type"] + " " + v["name"] + ";\n\t")

        if(len(i["fields"]) == 0):
            file.write("uint8 size() { return 0; }\n\t")
        else:
            file.write("uint8 size()\n\t{\n\t\treturn (")
            for v in i["fields"][:-1]:
                if(v["type"][:9] == "etk::List"):
                    file.write(v["name"] + ".size()+1")
                elif(v["type"][:17] == "etk::StaticString"):
                    file.write(v["name"] + ".length()")
                else:
                    file.write("sizeof(" + v["name"] + ") + \n\t\t")

            if(i["fields"][-1]["type"][:9] == "etk::List"):
                file.write(i["fields"][-1]["name"] + ".size()+1);\n\t}\n\t")
            elif( i["fields"][-1]["type"][:17] == "etk::StaticString"):
                file.write( i["fields"][-1]["name"] + ".length());\n\t}\n\t")
            else:
                file.write("sizeof(" +  i["fields"][-1]["name"] + "));\n\t}\n\t")

        file.write(
            "bool ack_required() { return " + i["ack_required"] + "; }\n\t")
        file.write("uint16 get_id() { return id; }\n\t")

        file.write("uint16 to_bytes(uint8* bytes)\n\t{\n\t\t")
        file.write("uint16 pos = 4;\n\t\t")
        file.write("uint8 temp = 0;\n\t\t")
        file.write("base_pack::gen_header(bytes);\n\t\t")
        if(len(i["fields"]) > 0):
            
            for v in i["fields"]:
                if(is_an_enum(v["type"], data) == True):
                    file.write("temp = static_cast<uint8>(" + v["name"] + ");\n\t\t")
                    file.write("base_pack::to_bytes(bytes, pos, temp);\n\t\t")
                else:
                    file.write("base_pack::to_bytes(bytes, pos, ")
                    file.write(v["name"] + ");\n\t\t")
        file.write("return pos;\n\t}\n\t")

        file.write("void from_bytes(uint8* bytes)\n\t{\n\t")
        file.write("uint8 temp = 0;\n\t\t")
        if(len(i["fields"]) > 0):
            file.write("\tuint16 pos = 0;\n\t\t")
            for v in i["fields"]:
                if(is_an_enum(v["type"], data) == True):
                    file.write("base_pack::from_bytes(bytes, pos, temp);\n\t\t");
                    file.write(v["name"] + " = static_cast<" + v["type"] + ">(temp);\n\t\t")
                else:
                    file.write("base_pack::from_bytes(bytes, pos, ")
                    file.write(v["name"] + ");\n\t\t")
        file.write("}")

        file.write("\n};\n\n")

    file.close()


def build_handlers_header(data):
    file = open("c3/c3_both_handlers.h", "w")
    file.write("""#ifndef C3_PULLS_IN_BOTH_HANDLERS
#error Do not include c3_both_handlers.h. Only include "c3.h"
#endif
""")
    for i in data["packets"]:
        if(i["owner"] == "both"):
            file.write("case " + i["title"].upper() + "_PACK:\n{\n\t")
            file.write(i["title"].lower() + "_pack pack(sync_in);\n\t")
            file.write("pack.from_bytes(&(data_buf[4]));\n\t")
            if(i["setting"] == "true"):
                for v in i["fields"]:
                    file.write("settings." +
                               v["name"] + " = pack." + v["name"] + ";\n\t")
                file.write(
                    "static_cast<T*>(this)->on_setting_changed(msg_id);\n")
            else:
                file.write("static_cast<T*>(this)->" +
                           i["title"] + "_pack_handler(pack);\n")
            if(i["ack_required"] == "true"):
                file.write(
                    "send_ack(" + i["title"].upper() + "_PACK, sync_in);\n")
            file.write("}\nbreak;\n")
    file.close()

    file = open("c3/c3_remote_handlers.h", "w")
    file.write("""#ifndef C3_PULLS_IN_REMOTE_HANDLERS
#error Do not include c3_remote_handlers.h. Only include "c3.h"
#endif
""")
    for i in data["packets"]:
        if(i["owner"] == "local"):
            file.write("case " + i["title"].upper() + "_PACK:\n{\n\t")
            file.write(i["title"].lower() + "_pack pack(sync_in);\n\t")
            file.write("pack.from_bytes(&(data_buf[4]));\n\t")
            if(i["setting"] == "true"):
                for v in i["fields"]:
                    file.write("settings." +
                               v["name"] + " = pack." + v["name"] + ";\n\t")
                file.write(
                    "static_cast<T*>(this)->on_setting_changed(msg_id);\n\t")
            else:
                file.write("static_cast<T*>(this)->" +
                           i["title"] + "_pack_handler(pack);\n\t")
            if(i["ack_required"] == "true"):
                file.write(
                    "send_ack(" + i["title"].upper() + "_PACK, sync_in);\n")
            file.write("\n}\nbreak;\n")
    file.close()

    file = open("c3/c3_local_handlers.h", "w")
    file.write("""#ifndef C3_PULLS_IN_LOCAL_HANDLERS
#error Do not include c3_local_handlers.h. Only include "c3.h"
#endif
""")
    for i in data["packets"]:
        if(i["owner"] == "remote"):
            file.write("case " + i["title"].upper() + "_PACK:\n{\n\t")
            file.write(i["title"].lower() + "_pack pack(sync_in);\n\t")
            file.write("pack.from_bytes(&(data_buf[4]));\n\t")
            if(i["setting"] == "true"):
                for v in i["fields"]:
                    file.write("settings." +
                               v["name"] + " = pack." + v["name"] + ";\n\t")
                file.write(
                    "static_cast<T*>(this)->on_setting_changed(msg_id);\n")
            else:
                file.write("static_cast<T*>(this)->" +
                           i["title"] + "_pack_handler(pack);\n")
            if(i["ack_required"] == "true"):
                file.write(
                    "send_ack(" + i["title"].upper() + "_PACK, sync_in);\n")
            file.write("}\nbreak;\n")
    file.close()


def build_settings_header(data):
    file = open("c3/c3_settings.h", "w")
    file.write("""#ifndef C3_SETTINGS_H
#define C3_SETTINGS_H

#include <etk.h>
#include "c3.h"

namespace ccc
{

class Settings
{
public:
    static Settings& get()
    {
        static Settings settings;
        return settings;
    }

    """)
    for i in data["packets"]:
        if(i["setting"] == "true"):
            for v in i["fields"]:
                file.write(v["type"] + " " + v["name"] + ";\n\t")

    file.write("""
private:
    Settings() {}
    Settings(const Settings& s) { (void)(s); }
    ~Settings() { }
};

}

#endif
""")
    file.close()

if(len(sys.argv) < 2):
    print("No input file specified!")
    exit()


file = open(sys.argv[1], "r")
data = json.load(file)

build_enums_header(data)
build_packets_header(data)
build_handlers_header(data)
build_settings_header(data)
build_docs(data)
shutil.copy2("./c3.h", "./c3/c3.h")
