import os
import bisect
import c4d
from c4d import gui
from c4d import documents

script_directory = os.path.dirname(os.path.realpath(__file__))
output_prefix = "Mesh_"
output_suffix = ".hpp"
output_directory = os.path.join(script_directory, "../Projects/Game/Resources/")
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

def vectorToList(vec):
    return [vec.x,vec.y,vec.z]

class Vertex:

    def __init__(self, position, normal, color):
        self._position = position
        self._normal = normal
        self._color = color

    def __lt__(self,other):
        return compareChain(compareVector(self._position, other._position), compareVector(self._normal, other._normal), compareVector(self._color, other._color)) == CompareResult.SMALLER

    def __eq__(self, other):
        return self._position == other._position and self._normal == other._normal and self._color == other._color

    def getPosition(self):
        return vectorToList(self._position)

    def getNormal(self):
        return vectorToList(self._normal)

    def getColor(self):
        return vectorToList(self._color)

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

    def buildBuffers(self):
        vertices = self._buildVertices()
        indices = self._buildIndices()
        geometry = []
        shading = []
        for vert in vertices:
            geometry += vert.getPosition()
            shading += vert.getNormal()
            shading += vert.getColor()
        bufs = {}
        bufs["geometry"] = geometry
        bufs["shading"] = shading
        bufs["indices"] = indices
        return bufs

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

def makeMeshForObject(obj, scale=1.0, useBisect=True):
    if obj.GetType() == c4d.Opolygon:
        points = obj.GetAllPoints()
        phong_norms = obj.CreatePhongNormals()
        color_tag = obj.GetTag(c4d.Tvertexcolor)
        if color_tag is None:
            raise RuntimeError("Polygon object has no vertex color tag")
        color_data = color_tag.GetDataAddressR()
        mesh = BisectMeshBuilder() if useBisect else MeshBuilder()
        for poly_ind, poly in enumerate(obj.GetAllPolygons()):
            if not poly.IsTriangle():
                raise RuntimeError("Polygon object is not triangulated")
            poly_inds = [poly.a, poly.b, poly.c]
            poly_poss = []
            for vert_ind in range(3):
                poly_poss += [scale * points[poly_inds[vert_ind]]]
            if phong_norms is not None:
                poly_norms = [phong_norms[poly_ind * 4 + i] for i in range(3)]
            else:
                poly_norms = [calculateTriangleNormal(poly_poss)] * 3
            if color_tag.IsPerPointColor():
                poly_cols = []
                for vert_ind in range(3):
                    poly_cols += [c4d.VertexColorTag.GetPoint(color_data, None, None, poly_inds[vert_ind])]
            else:
                poly_cols_dict = c4d.VertexColorTag.GetPolygon(color_data, poly_ind)
                poly_cols = [poly_cols_dict.a, poly_cols_dict.b, poly_cols_dict.c]
            for vert_ind in range(3):
                mesh.add(Vertex(poly_poss[vert_ind], poly_norms[vert_ind], poly_cols[vert_ind]))
        return mesh
    else:
        raise RuntimeError("Object is not a polygon object")

def objToBufDict(obj, scale=1.0):
    return makeMeshForObject(obj, scale).buildBuffers()

def toCArray(ctype, varname, list, structlen=None):
    txt = ctype + " " + varname + "[] = {"
    fields = 0
    structs = 0
    for item in list:
        if structlen is not None:
            if fields == 0:
                if structs > 0:
                    txt += ","
                txt += "{"
        if fields > 0:
            txt += ","
        txt += str(item)
        fields += 1
        if structlen is not None and fields == structlen:
            txt += "}"
            fields = 0
            structs += 1
    txt += "};"
    return txt

def main():
    doc = documents.GetActiveDocument()
    objs = doc.GetActiveObjects(c4d.GETACTIVEOBJECTFLAGS_0)
    scale = 1.0 / 100.0
    
    print("-- C4D JSON mesh exporter for Neighborhood project --")
    
    objbufdicts = {}

    for obj in objs:
            try:
                objbufdict = objToBufDict(obj, output_scale)
                objbufdicts[obj.GetName()] = objbufdict
            except RuntimeError as ex:
                print("Error while exporting object '%s'" % obj.GetName())
                print(ex)
                return
    
    for name, objbufdict in objbufdicts.iteritems():
        filename = os.path.join(output_directory, output_prefix + name + output_suffix)
        with open(filename, "w") as out:
            txt = ""
            txt += toCArray("const float", name + "_Geometry", objbufdict["geometry"]) + "\n"
            txt += toCArray("const float", name + "_Shading", objbufdict["shading"], 6) + "\n"
            txt += toCArray("const int", name + "_Indices", objbufdict["indices"]) + "\n"
            out.write(txt)
        print("Successfully written mesh '%s'" % name)

    gui.MessageDialog("Exporting done successfully", c4d.GEMB_OK)

if __name__ == '__main__':
    main()
