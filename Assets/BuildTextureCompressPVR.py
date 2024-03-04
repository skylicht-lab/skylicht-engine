import os
import platform
from tinydb import TinyDB, Query
import os.path
import time
import json

textureExt = [".tga"]

systemName = platform.system()
appname = ""
if systemName == "Windows":
    appname = "win32/PVRTexToolCLI.exe"
elif systemName == "Darwin":
    appname = "darwin/PVRTexToolCLI"
else:
    appname = "linux/PVRTexToolCLI"

compressTools = "../Tools/PVRTexTool/" + appname;

if systemName == "Windows":
    compressTools = compressTools.replace("/", "\\")

db = TinyDB('CachePVR.json')
fileQuery = Query()

print("---------------------------------------------")
print("COMPRESS TEXTURE TO PVRTC")
print("Delete CachePVR.json if you want rebuild all")
print("---------------------------------------------")
print("")


def compress(inputFile, outputFile):
    params = "-q pvrtcfast -f PVRTC1_4 -m"
    command = "%s %s -i %s -o %s" % (
        compressTools, params, inputFile, outputFile
    )
    print(command)
    os.system(command)


def needBuildTexture(filename):
    for fileType in textureExt:
        if filename.endswith(fileType):
            return True
    return False


def buildCompressTexture(dirName):
    outputFile = None
    inputFile = None

    for root, dirs, files in os.walk(dirName):
        for file in files:
            if needBuildTexture(file):
                outputFile = file
                outputFile = outputFile.replace(".tga", ".pvr")

                print("%s <- %s" % (outputFile, file))

                outputFile = root + "/" + outputFile
                inputFile = root + "/" + file

                needBuild = True
                modifyTime = time.ctime(os.path.getmtime(inputFile))

                queryInfo = db.get(fileQuery.file == inputFile)
                if queryInfo != None:
                    if queryInfo["mtime"] == modifyTime:
                        needBuild = False

                if needBuild:
                    compress(inputFile, outputFile)
                    if queryInfo != None:
                        db.update({'mtime': modifyTime},
                                  fileQuery.file == inputFile)
                    else:
                        db.insert({'file': inputFile, 'mtime': modifyTime})
                else:
                    print("- Cached")
                print("")


def main():
    directory = "."
    for filename in os.listdir(directory):
        if os.path.isdir(filename):
            buildCompressTexture(filename)
            continue
        else:
            continue


if __name__ == '__main__':
    main()
