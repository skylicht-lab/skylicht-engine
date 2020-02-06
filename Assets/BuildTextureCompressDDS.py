import os

textureExt = [".tga"]
compressTools = "..\\Tools\\NvTools\\nvcompress.exe"


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

                # call build tools
                params = "-bc1 -nocuda"
                command = "%s %s %s %s" % (
                    compressTools, params, inputFile, outputFile
                )
                print(command)
                os.system(command)


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
