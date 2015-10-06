f=open('LOG1.txt','r')
filename = 'interpolation_adaptive.txt'

raw_text=f.readlines()

index = 2
dist_dict = {5:0, 10:3, 11:6, 14:9, 17:12, 19:15, 20:18, 27:21}
calib_dict = {5:[0.8716,102], 10:[0.8595,116.4], 11:[0.9114,103.4], 14:[0.95,72.4], 17:[1,0], 19:[1.049,19.66], 20:[1.01,-10.6], 27:[0.87,153]}

array_info = []
text_info = '"x" "y" "z" "temp"\n'
num_messages = {5:0, 10:0, 11:0, 14:0, 17:0, 19:0, 20:0, 27:0}
total_messages = 0


while(index < len(raw_text)):
	if (int(raw_text[index].split()[2]) in dist_dict) and (int(raw_text[index+1].split()[2]) <= 1024):
		x = dist_dict[int(raw_text[index].split()[2])]
		y = float(int(raw_text[index-1])-int(raw_text[1]))/10000
		z = 0
		temp = int(raw_text[index+1].split()[2])
		calib = calib_dict[int(raw_text[index].split()[2])]
		new_temp = (temp*calib[0]) + calib[1]
		array_info.append([x,y,z,new_temp])
		if int(raw_text[index+2].split()[2]) < 650:
			num_messages[int(raw_text[index].split()[2])] = int(raw_text[index+2].split()[2])
	index = index + 6

total_messages = num_messages.values()
total_messages = sum(total_messages)

for i in range(0,len(array_info)):
	text_info = text_info + str(array_info[i][0]) + " " + str(array_info[i][1]) + " " + str(array_info[i][2]) + " " + str(array_info[i][3]) + "\n"

g = open(filename,'w')
g.write(text_info)

f.close()
g.close()
