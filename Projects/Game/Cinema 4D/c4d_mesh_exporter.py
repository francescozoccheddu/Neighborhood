import os
import bisect
import c4d
from c4d import gui
from c4d import documents

script_directory = os.path.dirname(os.path.realpath(__file__))
output_filename = "Mesh_{}.cpp"
output_directory = os.path.join(script_directory, "../Source Files/")
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

class Mesh:

    def __init__(self, geometry, shading, indices):
        self.geometry = geometry
        self.shading = shading
        self.indices = indices

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

    def buildMesh(self):
        vertices = self._buildVertices()
        indices = self._buildIndices()
        geometry = []
        shading = []
        for vert in vertices:
            geometry += vert.getPosition()
            shading += vert.getNormal()
            shading += vert.getColor()
        return Mesh(geometry,shading,indices)

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
        meshb = BisectMeshBuilder() if useBisect else MeshBuilder()
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
                poly_cols = [poly_cols_dict["a"], poly_cols_dict["b"], poly_cols_dict["c"]]
            for vert_ind in range(3):
                meshb.add(Vertex(poly_poss[vert_ind], poly_norms[vert_ind], poly_cols[vert_ind].GetVector3()))
        return meshb.buildMesh()
    else:
        raise RuntimeError("Object is not a polygon object")

def toCSV(list,strconv):
    csv = ""
    first = True
    for i,item in enumerate(list):
        if i > 0:
            csv += ","
        csv += strconv(item)
    return csv

def toFloat(x):
    return str(x) + "f"

def toInt(x):
    return str(x)

def toIndicesVector(list):
    return "{" + toCSV(list, toInt) + "}"

def toGeometryVector(list):
    cpp = ""
    cpp += "{"
    for v in range(0,len(list),3):
        if v > 0:
            cpp += ","
        cpp += "{{" + toCSV(list[v:v + 3], toFloat) + "}}"
    cpp += "}"
    return cpp

def toShadingVector(list):
    cpp = ""
    cpp += "{"
    for v in range(0,len(list),6):
        if v > 0:
            cpp += ","
        cpp += "{{" + toCSV(list[v:v + 3], toFloat) + "},{" + toCSV(list[v + 3:v + 6], toFloat) + "}}"
    cpp += "}"
    return cpp
            
def toCppMesh(mesh):
    cpp = ""
    cpp += "{" + "\n"
    cpp += toIndicesVector(mesh.indices) + ",\n"
    cpp += toGeometryVector(mesh.geometry) + ",\n"
    cpp += toShadingVector(mesh.shading) + "\n"
    cpp += "};\n"
    return cpp

def main():
    doc = documents.GetActiveDocument()
    objs = doc.GetActiveObjects(c4d.GETACTIVEOBJECTFLAGS_0)
    scale = 1.0 / 100.0
    
    print("-- C4D JSON mesh exporter for Neighborhood project --")
    
    meshes = {}

    for obj in objs:
        try:
            mesh = makeMeshForObject(obj, output_scale)
            meshes[obj.GetName()] = mesh
        except RuntimeError as ex:
            print("Error while exporting object '{}'".format(obj.GetName()))
            print(ex)
            return

    projname = doc.GetDocumentName()
    filename = output_filename.format(projname)
    filepath = os.path.join(output_directory, filename)
    
    cpp = "// Cinema 4D Exporter" + "\n"
    cpp += "// Project '{}'".format(projname) + "\n"
    cpp += "#include <Game/Meshes/Meshes.hpp>" + "\n"

    for name, mesh in meshes.iteritems():
        cpp += "// {}".format(name) + "\n"
        cpp += "const Mesh Mesh_{}".format(name) + "\n"
        cpp += toCppMesh(mesh)                   

    with open(filepath, "w") as out:
        out.write(cpp)
        print("Successfully written to '{}'".format(filepath))

    gui.MessageDialog("Exporting done successfully", c4d.GEMB_OK)

if __name__ == '__main__':
    main()
