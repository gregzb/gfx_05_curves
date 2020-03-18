script_file = open('script', 'w')

for i in range(1, 25):
    filenum = str(i).zfill(3)
    curves = []
    with open(f"dog/dog{filenum}.curves.txt") as f:
        curves = f.read().strip().split('\n')

    # curves = [curve.split(' ')[1:] for curve in curves]
    # for i in range(len(curves)):
    #     bigtemp = []
    #     temp = []
    #     for j in range(len(curves[i])):
    #         temp.append(curves[i][j])
    #         if len(temp) == 3:
    #             bigtemp.append(temp)
    #             temp = []
    #     curves[i] = bigtemp

    # print(curves)

    curves = [curve.strip().split('|') for curve in curves]
    for i in range(len(curves)):
        curve = curves[i][1:]

        for j in range(len(curve)):
            spline = curve[j]
            spline = spline.strip().split(':')
            spline = [val for val in spline if val]
            spline = [list(map(lambda x: float(x), point.split(','))) for point in spline]
            curve[j] = spline
        curves[i] = curve

    for curve in curves:
        script_file.write("clear\n")
        for spline in curve:
            upto = len(spline)-1
            if len(spline) > 10:
                upto += 1
            for j in range(upto):
                point1 = spline[j]
                left1 = point1[:2]
                co1 = point1[3:5]
                right1 = point1[6:8]

                point2 = spline[(j+1)%len(spline)]
                left2 = point2[:2]
                co2 = point2[3:5]
                right2 = point2[6:8]

                script_file.write("bezier\n")
                script_file.write(f"{co1[0] * 400 + 270} {co1[1] * 400 + 240} {right1[0] * 400 + 270} {right1[1] * 400 + 240} {left2[0] * 400 + 270} {left2[1] * 400 + 240} {co2[0] * 400 + 270} {co2[1] * 400 + 240}\n")
                #print(co1[0], co1[1], right1[0], right1[1], left2[0], left2[1], co2[0], co2[1])
        script_file.write(f"save\nimg{filenum}.png\n")
            #print()

script_file.close()