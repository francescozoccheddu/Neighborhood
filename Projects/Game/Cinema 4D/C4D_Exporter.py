import json
import os
import bisect
import c4d
from c4d import gui
from c4d import documents

outpath_desc_filename = "c4d_exporter_path"

class CompareResult:

    SMALLER = 1,
    EQUAL = 0,
    GREATER = -1

def compare(a,b):
    if a < b:
        return CompareResult.SMALLER
    if a > b:
        return CompareResult.GREATER
    return CompareResult.EQUAL

def compareChain(*ress):
    for res in ress:
        if res == CompareResult.SMALLER:
            return CompareResult.SMALLER
        if res == CompareResult.GREATER:
            return CompareResult.GREATER
    return CompareResult.EQUAL

def compareVector(a,b):
    return compareChain(compare(a.x,b.x), compare(a.y,b.y), compare(a.z,b.z))

def compareList(a,b):
    for ea, eb in zip(a,b):
        if ea < eb:
            return CompareResult.SMALLER
        elif ea > eb:
            return CompareResult.GREATER 
    return CompareResult.EQUAL

def vectorToDict(vec):
    return {
            "x" : vec.x,
            "y" : vec.y,
            "z" : vec.z
        }

class Vertex:

    def __init__(self, position, normal, textcoord):
        self._position = position
        self._normal = normal
        self._textcoord = textcoord

    def __lt__(self,other):
        return compareChain(compareVector(self._position, other._position), compareVector(self._normal, other._normal), compareList(self._textcoord, other._textcoord)) == CompareResult.SMALLER

    def __eq__(self, other):
        return self._position == other._position and self._normal == other._normal and self._textcoord == other._textcoord

    def toDict(self):
        return {
            "position" : vectorToDict(self._position),
            "normal" : vectorToDict(self._normal),
            "textcoord" : {
                    "x" : self._textcoord[0],
                    "y" : self._textcoord[1]
                } if self._textcoord is not None else None
        }

class MeshBuilder:

    def __init__(self):
        self._vertices = []
        self._indices = []

    def _buildVertices(self):
        return self._vertices

    def _buildIndices(self):
        return self._indices

    def add(self, vert):
        for i, v in enumerate(self._vertices):
            if v == vert:
                self._indices.append(i)
                return
        self._indices.append(len(self._vertices))
        self._vertices.append(vert)

    def toDict(self):
        return {
            "vertices" : list(map(lambda v: v.toDict(), self._buildVertices())),
            "indices" : self._buildIndices()
            }

class BisectMeshBuilder(MeshBuilder):

    def __init__(self):
        self._flat_inds = []
        return MeshBuilder.__init__(self)
    
    def _trySkip(self, vert, ind):
        if ind < len(self._vertices) and self._vertices[ind] == vert:
            self._indices.append(self._flat_inds[ind])
            return True
        return False

    def add(self, vert):
        insp = bisect.bisect_left(self._vertices, vert)
        if self._trySkip(vert, insp) or self._trySkip(vert, insp + 1):
            return
        self._flat_inds.insert(insp, len(self._vertices))
        self._indices.append(len(self._vertices))
        self._vertices.insert(insp, vert)

    def _buildVertices(self):
        verts = [None for _ in range(len(self._vertices))]
        for ind, flat_ind in enumerate(self._flat_inds):
            verts[flat_ind] = self._vertices[ind]
        return verts
   
def calculateTriangleNormal(verts):
    v = verts[1] - verts[0]
    w = verts[2] - verts[0]
    return v.Cross(w).GetNormalized()


def getTextCoordFromVector(vec):
    if vec.z != 0:
        raise ValueError("Unexpected UVW mapping")
    return [vec.x, vec.y]

def makeMeshForObject(obj, scale=1.0, useBisect=True):
    if obj.GetType() == c4d.Opolygon:
        points = obj.GetAllPoints()
        phong_norms = obj.CreatePhongNormals()
        uvw_tag = obj.GetTag(c4d.Tuvw)
        mesh = BisectMeshBuilder() if useBisect else MeshBuilder()
        for poly_ind, poly in enumerate(obj.GetAllPolygons()):
            if not poly.IsTriangle():
                raise ValueError("Polygon object is not triangulated")
            poly_poss = [points[poly.a] * scale, points[poly.b] * scale, points[poly.c] * scale]
            if phong_norms is not None:
                poly_norms = [phong_norms[poly_ind * 4 + i] for i in range(3)]
            else:
                poly_norms = [calculateTriangleNormal(poly_poss)] * 3
            if uvw_tag is not None:
                uvw = uvw_tag.GetSlow(poly_ind)
                poly_uvs = [getTextCoordFromVector(uvw[key]) for key in ["a","b","c"]]
            else:
                poly_uvs = [None] * 3
            for vert_ind in range(3):
                mesh.add(Vertex(poly_poss[vert_ind], poly_norms[vert_ind], poly_uvs[vert_ind]))
        #print("Verts %d Inds %d" % (len(mesh._vertices),len(mesh._indices)))
        return mesh
    else:
        raise ValueError("Object is not a polygon object")

def objToDict(obj, scale=1.0):
    return makeMeshForObject(obj, scale).toDict()

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
    jobjmap = {}
    
    print("-- C4D Export to JSON for Neighborhood project --")
    
    for obj in objs:
        try:
            jobj = objToDict(obj, scale)
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
