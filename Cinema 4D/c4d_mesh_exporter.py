import json
import os
import bisect
import c4d
from c4d import gui
from c4d import documents

script_directory = os.path.dirname(os.path.realpath(__file__))
output_directory = os.path.join(script_directory, "../Projects/Game/Resources/Meshes/")
output_ext = ".json"
output_scale = 1.0 / 100.0

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

def vectorToList(vec):
    return [vec.x,vec.y,vec.z]

class Vertex:

    def __init__(self, position, normal, textcoord):
        self._position = position
        self._normal = normal
        self._textcoord = textcoord

    def __lt__(self,other):
        return compareChain(compareVector(self._position, other._position), compareVector(self._normal, other._normal), compareList(self._textcoord, other._textcoord)) == CompareResult.SMALLER

    def __eq__(self, other):
        return self._position == other._position and self._normal == other._normal and self._textcoord == other._textcoord

    def toList(self):
        return vectorToList(self._position) + vectorToList(self._normal) + self._textcoord

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
            "verts" : list(map(lambda v: v.toList(), self._buildVertices())),
            "inds" : self._buildIndices()
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
        raise RuntimeError("Unexpected UVW mapping")
    return [vec.x, vec.y]

def makeMeshForObject(obj, scale=1.0, useBisect=True):
    if obj.GetType() == c4d.Opolygon:
        points = obj.GetAllPoints()
        phong_norms = obj.CreatePhongNormals()
        uvw_tag = obj.GetTag(c4d.Tuvw)
        mesh = BisectMeshBuilder() if useBisect else MeshBuilder()
        for poly_ind, poly in enumerate(obj.GetAllPolygons()):
            if not poly.IsTriangle():
                raise RuntimeError("Polygon object is not triangulated")
            poly_poss = [points[poly.a] * scale, points[poly.b] * scale, points[poly.c] * scale]
            if phong_norms is not None:
                poly_norms = [phong_norms[poly_ind * 4 + i] for i in range(3)]
            else:
                poly_norms = [calculateTriangleNormal(poly_poss)] * 3
            if uvw_tag is not None:
                uvw = uvw_tag.GetSlow(poly_ind)
                poly_uvs = [getTextCoordFromVector(uvw[key]) for key in ["a","b","c"]]
            else:
                poly_uvs = [0] * 3
            for vert_ind in range(3):
                mesh.add(Vertex(poly_poss[vert_ind], poly_norms[vert_ind], poly_uvs[vert_ind]))
        return mesh
    else:
        raise RuntimeError("Object is not a polygon object")

def objToDict(obj, scale=1.0):
    return makeMeshForObject(obj, scale).toDict()

def main():
    doc = documents.GetActiveDocument()
    objs = doc.GetActiveObjects(c4d.GETACTIVEOBJECTFLAGS_0)
    scale = 1.0 / 100.0
    
    print("-- C4D JSON mesh exporter for Neighborhood project --")
    
    jobjs = {}

    for obj in objs:
            try:
                jobj = objToDict(obj, output_scale)
                jobjs[obj.GetName()] = jobj
            except RuntimeError as ex:
                print("Error while exporting object '%s'" % obj.GetName())
                print(ex)
                return
    
    for name, jobj in jobjs.iteritems():
        filename = os.path.join(output_directory, name + output_ext)
        with open(filename, "w") as out:
            out.write(json.dumps(jobj, separators=(',', ':'), indent=None))
        print("Successfully written mesh '%s'" % name)

    gui.MessageDialog("Exporting done successfully", c4d.GEMB_OK)

if __name__ == '__main__':
    main()
