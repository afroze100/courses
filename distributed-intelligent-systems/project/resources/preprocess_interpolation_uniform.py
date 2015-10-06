f=open('LOG1.txt','r')
filename = 'interpolation_uniform.txt'

raw_text=f.readlines()

index = 2
dist_dict = {30:0, 23:3, 21:6, 18:9, 13:12, 9:15, 8:18, 2:21}
calib_dict = {30:[0.75,144], 23:[0.75,164], 21:[0.98,94.4], 18:[0.94,69], 13:[0.75,193], 9:[0.8839,69], 8:[0.918,77.2], 2:[0.845,187.5]}

array_info = []
text_info = '"x" "y" "z" "temp"\n'
num_messages = {30:0, 23:0, 21:0, 18:0, 13:0, 9:0, 8:0, 2:0}
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
