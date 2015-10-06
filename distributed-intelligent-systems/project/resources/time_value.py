f=open('/home/gmilani/myfiles/dis/Project/DIS_Project/Calib/LOG1.txt','r');
raw_text=f.readlines();

index=2;
array_ref = []; ## aray containing refernce node readings
array_cal = [[], [], [], [], [], [], []];	## array containing to-be-calibrated node readings
values_r = [[], [], [], [], [], [], []];
values_c = [[], [], [], [], [], [], []];

while(index < len(raw_text)):
	
	if int(raw_text[index].split()[2]) == 17:
		array_ref.append([int(raw_text[index-1].split()[0]),int(raw_text[index+1].split()[2])])
	elif int(raw_text[index].split()[2]) == 5:
		array_cal[0].append([int(raw_text[index-1].split()[0]),int(raw_text[index+1].split()[2])])
	elif int(raw_text[index].split()[2]) == 10:
		array_cal[1].append([int(raw_text[index-1].split()[0]),int(raw_text[index+1].split()[2])])
	elif int(raw_text[index].split()[2]) == 11:
		array_cal[2].append([int(raw_text[index-1].split()[0]),int(raw_text[index+1].split()[2])])
	elif int(raw_text[index].split()[2]) == 14:
		array_cal[3].append([int(raw_text[index-1].split()[0]),int(raw_text[index+1].split()[2])])
	elif int(raw_text[index].split()[2]) == 19:
		array_cal[4].append([int(raw_text[index-1].split()[0]),int(raw_text[index+1].split()[2])])
	elif int(raw_text[index].split()[2]) == 20:
		array_cal[5].append([int(raw_text[index-1].split()[0]),int(raw_text[index+1].split()[2])])
	elif int(raw_text[index].split()[2]) == 27:
		array_cal[6].append([int(raw_text[index-1].split()[0]),int(raw_text[index+1].split()[2])])
	'''
	if int(raw_text[index].split()[2]) == 11:
		array_ref.append([int(raw_text[index-1].split()[0]),int(raw_text[index+1].split()[2])])
	elif int(raw_text[index].split()[2]) == 5:
		array_cal[0].append([int(raw_text[index-1].split()[0]),int(raw_text[index+1].split()[2])])
	elif int(raw_text[index].split()[2]) == 17:
		array_cal[1].append([int(raw_text[index-1].split()[0]),int(raw_text[index+1].split()[2])])
	'''
	index = index + 6




delta = 900

files = ["5","10","11","14","20","19","27"]

for i in range(0,len(array_cal)):
	r,c = 0,0
	while r < len(array_ref) and c < len(array_cal[i]):
		if array_ref[r][0] < array_cal[i][c][0] - delta:
			r = r + 1
		elif array_cal[i][c][0] < array_ref[r][0] - delta:
			c = c + 1
		else:
			values_r[i].append([array_ref[r][0], array_ref[r][1]])
			values_c[i].append([array_cal[i][c][0], array_cal[i][c][1]])
			r, c = r+1, c+1
	

	print("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%calibrated ", files[i], " against 11")
	print(values_r[i])
	print(values_c[i])
	filename_ref = 'reference_' + files[i] + '.txt';
	filename_cal = 'calibrate_' + files[i] + '.txt';
	g=open(filename_ref,'w');
	h=open(filename_cal,'w');
	g.write(str(values_r[i]));
	h.write(str(values_c[i]));
	g.close()
	h.close()

	
