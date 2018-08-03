import json
import os
import math
import bisect
import c4d
from c4d import gui
from c4d import documents

outpath_desc_filename = "c4d_exporter_path"

def isCloseToNumber(a,b,tollerance=0.0):
    return a == b if tollerance == 0.0 else math.isclose(a,b, abs_tol = tollerance)

def isCloseToVector(a,b,tollerance=0.0):
    return a == b if tollerance == 0.0 else (isCloseToNumber(a.x,b.x,tollerance) and isCloseToNumber(a.y,b.y,tollerance) and isCloseToNumber(a.x,b.y,tollerance))

class CompareResult:

    SMALLER = 1,
    EQUAL = 0,
    GREATER = -1

def compare(a,b):
    if a < b:
        return CompareResult.SMALLER
    if a == b:
        return CompareResult.EQUAL
    return CompareResult.GREATER

def compareChain(*ress):
    for res in ress:
        if res == CompareResult.SMALLER:
            return CompareResult.SMALLER
        if res == CompareResult.GREATER:
            return CompareResult.GREATER
    return CompareResult.EQUAL

def compareVector(a,b):
    return compareChain(compare(a.x,b.x), compare(a.y,b.y), compare(a.z,b.z))

def vectorToDict(vec):
    return {
            "x" : vec.x,
            "y" : vec.y,
            "z" : vec.z
        }

class Vertex:

    def __init__(self, position, normal):
        self._position = position
        self._normal = normal

    def __lt__(self, other):
        return compareChain(compareVector(self._position, other._position), compareVector(self._normal, other._normal)) == CompareResult.SMALLER

    def __eq__(self, other):
        return self._position == other._position and self._normal == other._normal

    def isCloseTo(self, other, tollerance=0.0):
        return self == other if tollerance == 0.0 else (isCloseToVector(self._position, other._position, tollerance) and isCloseToVector(self._normal, other._normal, tollerance))

    def toDict(self):
        return {
            "position" : vectorToDict(self._position),
            "normal" : vectorToDict(self._normal)
        }

    def __format__(self, format_spec):
        return self.__str__()

    def __repr__(self):
        return self.__str__()

    def __str__(self):
        return "<Pos: {} Norm: {}>".format(str(self._position), str(self._normal))

class VertexSortHelper(Vertex):

    def __init__(self, vert, index):
        self._index = index
        return Vertex.__init__(self, vert._position, vert._normal)

    def getIndex(self):
        return self._index

class MeshBuilder:

    def __init__(self):
        self._vertices = []
        self._indices = []

    def append(self, vert):
        self._indices.append(len(self._vertices))
        self._vertices.append(vert)

    def getVertices(self):
        return self._vertices

    def getIndices(self):
        return self._indices

    def toDict(self):
        return {
            "vertices" : list(map(lambda v: v.toDict(), self._vertices)),
            "indices" : self._indices
            }

class LinearMeshBuilder(MeshBuilder):

    def __init__(self):
        self.dups = 0 # TODO
                      # Deletemeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee!
        MeshBuilder.__init__(self)

    def add(self, vert, tollerance=0.0):
        for pind, pvert in enumerate(self._vertices):
            if pvert.isCloseTo(vert, tollerance):
                self._indices += [pind]
                self.dups +=1
                return
        self.append(vert)

class BisectMeshBuilder(LinearMeshBuilder):

    def __init__(self):
        self._verticesh = []
        return LinearMeshBuilder.__init__(self)

    def add(self, vert, tollerance=0.0):
        ii = bisect.bisect_left(self._verticesh, vert)
        if ii < len(self._verticesh) and self._verticesh[ii].isCloseTo(vert, tollerance):
            self._indices.append(self._verticesh[ii].getIndex())
            self.dups+=1
            return
        elif ii + 1 < len(self._verticesh) and self._verticesh[ii + 1].isCloseTo(vert, tollerance):
            self._indices.append(self._verticesh[ii + 1].getIndex())
            self.dups+=1
            return
        self._verticesh.insert(ii,VertexSortHelper(vert,len(self._vertices)))
        self.append(vert)            

def getTriangleIndices(obj):
    inds = []
    for poly in obj.GetAllPolygons():
        if poly.IsTriangle():
            inds += [poly.a, poly.b, poly.c]
        else:
            raise ValueError("Polygon object is not triangulated")
    return inds

def getPerTriangleIndexPhongNormals(obj):
    pnorms = obj.CreatePhongNormals()
    if pnorms is None:
        return None
    tnorms = []
    for pi, pnorm in enumerate(pnorms):
        if not pi % 4 > 2:
            tnorms.append(pnorm)
    return tnorms
   
def calculatePerTriangleNormals(obj):
    norms = []
    points = obj.GetAllPoints()
    for poly in obj.GetAllPolygons():
        if poly.IsTriangle():
            a = points[poly.a]
            b = points[poly.b]
            c = points[poly.c]
            v = b - a
            w = c - a
            norms.append(v.Cross(w).GetNormalized())
        else:
            raise ValueError("Polygon object is not triangulated")
    return norms

def makeMeshForObject(obj,scale=1.0, tollerance=0.0, useBisect=True):
    if obj.GetType() == c4d.Opolygon:
        inds = getTriangleIndices(obj)
        points = obj.GetAllPoints()
        norms = getPerTriangleIndexPhongNormals(obj)
        if norms is None:
            phong = False
            norms = calculatePerTriangleNormals(obj)
        else:
            phong = True
        mesh = BisectMeshBuilder() if useBisect else LinearMeshBuilder()
        for ii, ind in enumerate(inds):
            position = points[ind] * scale
            if phong:
                normal = norms[ii]
            else:
                normal = norms[ii / 3]
            vert = Vertex(position,normal)
            mesh.add(vert, tollerance)
        print("Found %d dups" % mesh.dups)
        return mesh
    else:
        raise ValueError("Object is not a polygon object")

def objToDict(obj, scale=1.0, tollerance=0.0):
    return makeMeshForObject(obj,scale,tollerance).toDict()

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
    scale = 1.0 / 100.0
    tollerance = 0.0
    jobjmap = {}
    
    print("-- C4D Export to JSON for Neighborhood project --")
    
    for obj in objs:
        try:
            jobj = objToDict(obj,scale,tollerance)
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
    

if __name__ == '__main__':
    main()
