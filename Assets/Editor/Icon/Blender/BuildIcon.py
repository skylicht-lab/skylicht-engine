from PIL import Image
import os


def export(source, target, startx, starty, cellw, cellh, numx, numy, padding):
    image = Image.open(source)
    offsetx = startx
    offsety = starty
    for y in range(0, numy):
        offsetx = startx
        offsety = starty + y * (cellh + padding)
        for x in range(0, numx):
            d = divmod(y, 26)
            if d[0] > 0:
                row_name = ("%c%c" % (65 + d[1], 65 + d[0] - 1))
            else:
                row_name = ("%c" % (65 + d[1]))            
            cropped_image = image.crop(
                (offsetx, offsety, offsetx + cellw, offsety + cellh))
            cropped_image.save(target + ("%s_%d" % (row_name, x + 1)) + ".png")
            offsetx = offsetx + cellw + padding        


def main():
    if (os.path.exists("./32/") is False):
        os.mkdir("./32/")
    export("icon_32.png", "./32/", 6, 16, 40, 40, 26, 30, 2)

    if (os.path.exists("./16/") is False):
        os.mkdir("./16/")
    export("icon_16.png", "./16/", 3, 8, 20, 20, 26, 30, 1)


if __name__ == '__main__':
    main()
