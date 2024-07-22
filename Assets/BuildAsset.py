import zipfile
import os

compressExt = ["xml", "ttf", "otf", "txt",
               "spritedata","gui",
               "mat",
               "hlsl", "glsl",
               "png", "jpg",
               "smesh", "sanim",
               "mp3", "wav",
               "font"]
compressResourceModel = ["dae", "obj", "mtl", "fbx"]
compressTextureDDSExt = ["dds"]
compressTexturePVRExt = ["pvr"]
compressTextureETCExt = ["etc2"]


def needCompress(filename):
    for fileType in compressExt:
        if filename.endswith(fileType):
            return True
    return False


def needCompressResource(filename):
    for fileType in compressResourceModel:
        if filename.endswith(fileType):
            return True
    return False


def needCompressDDS(filename):
    for fileType in compressTextureDDSExt:
        if filename.endswith(fileType):
            return True
    return False


def needCompressPVR(filename):
    for fileType in compressTexturePVRExt:
        if filename.endswith(fileType):
            return True
    return False


def needCompressETC(filename):
    for fileType in compressTextureETCExt:
        if filename.endswith(fileType):
            return True
    return False


def compress(dirName):
    outputZip = "../Bin/" + dirName + ".zip"
    outputZipRes = "../Bin/" + dirName + "Resource.zip"
    outputZipDDS = "../Bin/" + dirName + "DDS.zip"
    outputZipPVR = "../Bin/" + dirName + "PVR.zip"
    outputZipETC = "../Bin/" + dirName + "ETC.zip"
    zipCommonFile = None
    zipResFile = None
    zipDDSFile = None
    zipPVRFile = None
    zipETCFile = None
    for root, dirs, files in os.walk(dirName):
        for file in files:
            if file.find("!") >= 0 or root.find("!") >= 0:
                print("Skip: %s - %s" % (root, file))
                continue
            if needCompress(file):
                if zipCommonFile == None:
                    zipCommonFile = zipfile.ZipFile(
                        outputZip, "w", zipfile.ZIP_DEFLATED)
                print("%s <-- %s - %s" % (outputZip, root, file))
                zipCommonFile.write(os.path.join(root, file))

            if needCompressResource(file):
                if zipResFile == None:
                    zipResFile = zipfile.ZipFile(
                        outputZipRes, "w", zipfile.ZIP_DEFLATED)
                print("%s <-- %s - %s" % (outputZipRes, root, file))
                zipResFile.write(os.path.join(root, file))

            if needCompressDDS(file):
                if zipDDSFile == None:
                    zipDDSFile = zipfile.ZipFile(
                        outputZipDDS, "w", zipfile.ZIP_DEFLATED)
                print("%s <-- %s - %s" % (outputZipDDS, root, file))
                zipDDSFile.write(os.path.join(root, file))

            if needCompressPVR(file):
                if zipPVRFile == None:
                    zipPVRFile = zipfile.ZipFile(
                        outputZipPVR, "w", zipfile.ZIP_DEFLATED)
                print("%s <-- %s - %s" % (outputZipPVR, root, file))
                zipPVRFile.write(os.path.join(root, file))

            if needCompressETC(file):
                if zipETCFile == None:
                    zipETCFile = zipfile.ZipFile(
                        outputZipETC, "w", zipfile.ZIP_DEFLATED)
                print("%s <-- %s - %s" % (outputZipETC, root, file))
                zipETCFile.write(os.path.join(root, file))

    if zipCommonFile != None:
        zipCommonFile.close()

    if zipResFile != None:
        zipResFile.close()

    if zipDDSFile != None:
        zipDDSFile.close()

    if zipPVRFile != None:
        zipPVRFile.close()

    if zipETCFile != None:
        zipETCFile.close()

