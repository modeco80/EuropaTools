#
# EuropaTools
#
# (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
#
# SPDX-License-Identifier: MIT
#

# This is a Model Researcher Pro script which can be used
# to automatically display .msh meshes inside of it.
#
# While I haven't written a native exporter yet, this should *not* 
# be considered a replacement for that, especially given the issues:
# - python (its slow as balls)
# - Model Researcher is Windows-only
# - Generally this is just a quick hack and it shows in code quality.

import mrp

bf = mrp.get_bfile()

# read '\r\n' terminated string
# why the fuck did they do this
def Storage_ReadLine():
    ret = ""
    rntest = b""
    while True:
        a = bf.reads("1s")[0]

        if a == b'\r' or a == b'\n':
            rntest += a
            if rntest == b'\r\n':
                return ret   
        else:
            ret += a.decode('ascii')
    
def ReadMeshBlock(readCallback):
    count = bf.readShort()
    data = None
    if count != 0:
          data = readCallback(count)
    return (count, data)

def ReadVec3(count):
    return bf.read(count * (3 * 4))

def ReadVertexColors(count):
    return bf.read(count * 4)

def ReadFaces(count):
    return bf.read(count * (3 * 2))
    
# main

# Make sure this is a MESH and it's a 2.0 version
# 1.2 version is actually text-format and I can't be bothered
# to support it.
if Storage_ReadLine() != 'MESH':
        raise ValueError('not a Europa msh')
if Storage_ReadLine() != '2.0':
        raise ValueError('not a Europa msh 2.0 (this parses only binary version 2.0)')

mesh_count = bf.readShort()
for i in range(0, mesh_count):
    mtlFilename = Storage_ReadLine()
    
    # read some pieces
    verts = ReadMeshBlock(ReadVec3)
    vertcolors = ReadMeshBlock(ReadVertexColors)
    normals = ReadMeshBlock(ReadVec3)

    # Need to manually ReadMeshBlock() here
    # since we need vert count to read the UV block
    uvCount = bf.readShort()
    uv = None
    if uvCount != 0:
        print(f' {uvCount} uvs')
        uv = []
        for i in range(0, uvCount):
            uv_bytes = bf.read(verts[0] * (2 * 4))
            uv.append(uv_bytes)

    faces = ReadMeshBlock(ReadFaces)
    
    # Add it and set everything up, add verts, faces,
    # and normals/UV if provided
    mesh = mrp.create_mesh(mtlFilename)
    mesh.set_vertices(verts[1], tp="Float")
    mesh.set_faces(faces[1], tp="Short")
    if normals[0] != 0:
        mesh.set_normals(normals[1], tp="Float")
    if uvCount != 0:
        mesh.set_uvs(uv[0], tp="Float")
    print(f'Read mesh {mtlFilename}')

mrp.render("All")
