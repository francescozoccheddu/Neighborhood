import json
import os
import c4d
from c4d import gui
from c4d import documents

def_outpath_filename = "c4d_exporter_path"

def jsonizePolygonObject(obj,scale=1.0):
    if obj.GetType() == c4d.Opolygon:
        jobj = {}
        jobj["name"] = obj.GetName()
        jobj["indices"] = []
        for poly in obj.GetAllPolygons():
            if poly.IsTriangle():
                jobj["indices"] += [poly.a, poly.b, poly.c]
            else:
                raise ValueError("Polygon object is not triangulated")
        jobj["vertices"] = []
        for vert in obj.GetAllPoints():
            svert = vert * scale
            jobj["vertices"] += [svert.x, svert.y, svert.z]
        return jobj
    else:
        raise ValueError("Object is not a polygon object")

def getOutputFilename():
    current_path = documents.GetActiveDocument().GetDocumentPath()
    if not current_path:
        current_path = os.path.dirname(os.path.realpath(__file__))
    outpath_filename = os.path.join(current_path, def_outpath_filename)
    if os.path.exists(outpath_filename):
        outpath_content = ""
        with open(outpath_filename) as outpath_file:
            outpath_content = outpath_file.read()
        outpath_content = outpath_content.strip()
        if outpath_content:
            if not os.path.isabs(outpath_content):
                outpath = os.path.join(current_path, outpath_content)
            else:
                outpath = outpath_content
            if os.access(os.path.dirname(outpath), os.W_OK):
                return outpath
            else:
                raise RuntimeError("File '%s' does not contains a valid writable output path" % outpath_filename)
    outpath = c4d.storage.SaveDialog(c4d.FILESELECTTYPE_ANYTHING, "Export scene", "", current_path, "")
    if outpath:
        return outpath
    else:
        raise RuntimeError("Save dialog cancelled")

def main():
    doc = documents.GetActiveDocument()
    objs = doc.GetActiveObjects(c4d.GETACTIVEOBJECTFLAGS_0)
    scale = 1.0/100.0
    jobjs = []
    
    for obj in objs:
        try:
            jobjs += [jsonizePolygonObject(obj,scale)]
        except ValueError as ex:
            print("Error while exporting object '%s': '%s'" % (obj.GetName(), ex.message))
    
    filename = getOutputFilename()
    with open(filename,"w") as out:
        out.write(json.dumps(jobjs))
        
    print("Successfully written to file '%s'" % filename)
    

if __name__=='__main__':
    main()
