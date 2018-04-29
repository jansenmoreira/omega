import xml.etree.ElementTree as xml
import argparse
import os
import zipfile

parser = argparse.ArgumentParser(description="Generate Utility/UCD.c")
parser.add_argument("--ucd", required=True, help="Zip file that contains 'ucd.all.flat.xml'")
parser.add_argument("--output", required=True, help="Folder to output Utility/UCD.c")
args = parser.parse_args()

general_categories = {
    "Lu":  0, "Ll":  1, "Lt":  2, "Lm":  3, "Lo":  4,
    "Mn":  5, "Mc":  6, "Me":  7, "Nd":  8, "Nl":  9,
    "No": 10, "Pc": 11, "Pd": 12, "Ps": 13, "Pe": 14,
    "Pi": 15, "Pf": 16, "Po": 17, "Sm": 18, "Sc": 19,
    "Sk": 20, "So": 21, "Zs": 22, "Zl": 23, "Zp": 24,
    "Cc": 25, "Cf": 26, "Cs": 27, "Co": 28, "Cn": 29,
}

numeric_type = {
    "None": 0, "De": 1, "Di": 2, "Nu": 3
}

ucd_xml_string = ""

with zipfile.ZipFile(args.ucd) as ucd_zip:
    with ucd_zip.open("ucd.all.flat.xml") as ucd:
        ucd_xml_string = ucd.read()

ucd_xml = xml.fromstring(ucd_xml_string)

namespace = { "unicode": "http://www.unicode.org/ns/2003/ucd/1.0" }

description = ucd_xml.find("unicode:description", namespace)
repertoire = ucd_xml.find("unicode:repertoire", namespace)

if not os.path.exists(args.output):
    os.mkdir(args.output)

with open(os.path.join(args.output, os.path.join("Utility", "UCD.c")), "w") as file:
    file.write("#include <Utility/Unicode.h>\n\n")
    file.write("// {}\n\n".format(description.text))
    file.write("const Unicode UCD[1114112] = {\n")

    for char in repertoire:

        gc = general_categories[char.attrib["gc"]]
        nt = numeric_type[char.attrib["nt"]]
        nv = 0 if char.attrib["nv"] == "NaN" or nt != 1 else int(char.attrib["nv"])
        XIDS = 1 if char.attrib["XIDS"] == "Y" else 0
        XIDC = 1 if char.attrib["XIDC"] == "Y" else 0
        
        try:
            cp = int(char.attrib["cp"], base=16)
            file.write("    {{ {}, {}, {}, {}, {} }},\n".format(gc, XIDS, XIDC, nt, nv))

        except KeyError:
            first = int(char.attrib["first-cp"], base=16)
            last = int(char.attrib["last-cp"], base=16)

            for it in range(first, last + 1):
                file.write("    {{ {}, {}, {}, {}, {} }},\n".format(gc, XIDS, XIDC, nt, nv))
    
    file.write("};\n")

