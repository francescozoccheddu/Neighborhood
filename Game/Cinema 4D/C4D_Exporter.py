import json
import os
import c4d
from c4d import gui
from c4d import documents

outpath_desc_filename = "c4d_exporter_path"

def jsonizePolygonObject(obj,scale=1.0):
    if obj.GetType() == c4d.Opolygon:
        jobj = {}
        jobj["indices"] = []
        for poly in obj.GetAllPolygons():
            if poly.IsTriangle():
                jobj["indices"] += [poly.a, poly.b, poly.c]
            else:
                raise ValueError("Polygon object is not triangulated")
        jobj["positions"] = []
        for vert in obj.GetAllPoints():
            svert = vert * scale
            jobj["positions"] += [svert.x, svert.y, svert.z]
        return jobj
    else:
        raise ValueError("Object is not a polygon object")

def getOutputFilenames():
    doc_path = documents.GetActiveDocument().GetDocumentPath()
    if not doc_path:
        doc_path = os.path.dirname(os.path.realpath(__file__))
    outpaths = []
    desc_filename = os.path.join(doc_path, outpath_desc_filename)
    if os.path.exists(desc_filename):
        outpaths = []
        with open(desc_filename) as desc_file:
            outpath_lines = desc_file.read().splitlines()
        for line in outpath_lines:
            sline = line.strip()
            if not os.path.isabs(sline):
                outpath = os.path.join(os.path.dirname(desc_filename), sline)
            else:
                outpath = sline
            if os.access(os.path.dirname(outpath), os.W_OK):
                outpaths += [outpath]
            else:
                raise RuntimeError("File '%s' contains an invalid writable output path '%s'" % (desc_filename, outpath))
    if not len(outpaths) > 0:
        outpath = c4d.storage.SaveDialog(c4d.FILESELECTTYPE_ANYTHING, "Export scene", "", doc_path, "")
        if not outpath:
            raise RuntimeError("Save dialog cancelled")
        outpaths = [outpath]
    return outpaths

def main():
    doc = documents.GetActiveDocument()
    objs = doc.GetActiveObjects(c4d.GETACTIVEOBJECTFLAGS_0)
    scale = 1.0/100.0
    jobjmap = {}
    
    print("-- C4D Export to JSON for Neighborhood project --")
    
    for obj in objs:
        try:
            jobj = jsonizePolygonObject(obj,scale)
            if obj.GetName() in jobjmap:
                print("Object name '%s' appears multiple times, only one object with this name will be exported" % obj.GetName())
            jobjmap[obj.GetName()] = jobj
        except ValueError as ex:
            print("Error while exporting object '%s': '%s'" % (obj.GetName(), ex.message))
    
    filenames = getOutputFilenames()
    for filename in filenames:
        with open(filename,"w") as out:
            out.write(json.dumps(jobjmap))
        print("Successfully written to file '%s'" % filename)
        
    print("Done")
    

if __name__=='__main__':
    main()
