# -*- coding: utf-8 -*-

from shutil import copytree,ignore_patterns,rmtree,copyfile
import os
import sys

ROOT_PATH = os.path.join("..", "..")  # baidu/of
CONFIG_MATERIAL_PATH = os.path.join(ROOT_PATH, "Data", "material")
cgbatchPath = os.path.join(ROOT_PATH, "bin", "CompilerShader")

includePath = os.path.join(CONFIG_MATERIAL_PATH, "common")

defaultRoots = [ os.path.join(CONFIG_MATERIAL_PATH, "2d") ]

roots = defaultRoots

if len(sys.argv) > 1:
    roots = sys.argv[1:]

platformList = [ "d3d11", "opengl", "gles2", "gles31" ]
apiList = [ "D3D11", "OPENGL", "GLES2", "GLES31" ]

def processShader(shaderPath):
    print("processing %s" % shaderPath)
    materialPath = shaderPath.replace(".shader", "")
    if os.path.isdir(materialPath):
        rmtree(materialPath)
    elif os.path.isfile(materialPath):
        os.remove(materialPath)
    os.mkdir(materialPath)
    for i in range(len(platformList)):
        platform = platformList[i]
        api = apiList[i]
        platformMaterialPath = os.path.join(materialPath, platform + ".lua")
        os.system("%s -i %s -o %s -inc %s -api %s" % 
            (cgbatchPath, shaderPath, platformMaterialPath, includePath, api))
    print("finished %s" % shaderPath)

def processDir(rootDir):
    for root, dirs, files in os.walk(rootDir):
        shaderFiles = filter(lambda name : name.endswith(".shader"), files)
        for shaderFile in shaderFiles:
            shaderPath = os.path.join(root, shaderFile)
            processShader(shaderPath)

for root in roots:
    if os.path.isdir(root):
        processDir(root)
    elif os.path.isfile(root) and root.endswith(".shader"):
        processShader(root)
    else:
        print("invalid path: %s" % root)

raw_input("GenMat done, enter any key to Exit")

