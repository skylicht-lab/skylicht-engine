import os
from tinydb import TinyDB, Query
import os.path
import time
import json

normalMap = ["_norm.", "_ddn.", "_n."]
textureExt = [".tga"]
compressTools = "..\\Tools\\NvTools\\nvcompress.exe"
db = TinyDB('CacheDDS.json')
fileQuery = Query()

print("---------------------------------------------")
print("COMPRESS TEXTURE TO DDS")
print("Delete CacheDDS.json if you want rebuild all")
print("---------------------------------------------")
print("")


def compress(inputFile, outputFile):
    format = "-bc1"
    for nmExt in normalMap:
        if inputFile.find(nmExt) >= 0:
            format = "-bc3"
    # call build tools
    params = "%s -nocuda" % (format)
    command = "%s %s %s %s" % (
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
                outputFile = outputFile.replace(".tga", ".dds")

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
