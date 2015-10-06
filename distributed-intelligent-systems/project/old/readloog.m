function [out] = readloog(logfile1, logfile2, offset, deg)
close all

log1 = load(logfile1);
out1 = log1(:,2:3);

log2 = load(logfile2);
out2 = log2(:,2:3);

figure(1)
subplot(3,1,1)
plot(out1(:,1)/1000,out1(:,2),'.b');
xlabel('Time [s]');
ylabel('Light sensor value');
title('Logged Signal 1 ');

subplot(3,1,2)

plot(out2(:,1)/1000,out2(:,2),'.r');
xlabel('Time [s]');
ylabel('Light sensor value');
title('Logged Signal 2 ');

subplot (3,1,3)
plot(out1(:,1)/1000,out1(:,2),'.b',out2(:,1)/1000,out2(:,2),'.r');


moy1=mean(out1(:,2))
moy2=mean(out2(:,2));

deviation1=std(out1(:,2));
deviation2=std(out2(:,2));

z1=((out1(:,2)-moy1)/deviation1);
z2=((out2(:,2)-moy2)/deviation2);

%la calibration se fait sur les valeurs du 1
offset1=floor(moy1-moy2)
%d'après ecransousjournal2 offset=-47
out2_calibre=out2(:,2)+offset;

% subplot (7,1,4)
% plot(out1(:,1)/1000,z1,'.b');
% xlabel('Time [s]');
% ylabel('Light sensor value');
% title('Logged normalized Signal 1 ');
% subplot (7,1,5)
% plot(out2(:,1)/1000,z2 ,'.r');
% xlabel('Time [s]');
% ylabel('Light sensor value');
% title('Logged normalized Signal 2 ');
% subplot (7,1,6)
% plot(out1(:,1)/1000,z1,'.b',out2(:,1)/1000,z2,'.r');

figure(2)
plot(out1(:,1)/1000,out1(:,2),'.b',out2(:,1)/1000,out2_calibre,'.r');
xlabel('Time [s]');
ylabel('Light sensor value');
title('signal 1 and signal 2 with offset ');

figure(3)
k=0;
diff_t=out1(1,1)-out2(1,1);

max_i=length(out1(:,2));
max_k=length(out2(:,2));

MAX=max([max_i max_k]);

out1bis=zeros(1,MAX);
out2bis=zeros(1,MAX);
l=1;

for i=1:1:max_i
    for k=1:1:max_k
    if(floor(out1(i,1)/1000) == floor(out2(k,1)/1000) )
        
      out1bis(l,1)=l;
      out2bis(l,1)=l;
      out1bis(l,2)=out1(i,2);
      out2bis(l,2)=out2(k,2);
      l=l+1;
      break
    end
    
    end   
end
figure(4)
plot(out1bis(:,2),'b');
hold on
plot(out2bis(:,2),'r');
figure(5)
plot(out1bis(:,2),out2bis(:,2),'.g');






for i=1:1:deg
out1_corr(i,:)=polyval(polyfit(out1bis(:,2),out2bis(:,2),i),out1bis(:,2));
end

figure(6)
for i=1:1:deg
subplot(floor(deg/4+1),4,i)
title('superposition avec valeurs corrigées 1er ordre')
plot(out2bis(:,1),out2bis(:,2),'r',out1bis(:,1),out1_corr(i,:),'b')
end

total_dif=zeros(deg,1);
total_dif_withoutcorr=sum(abs(out1bis(:,2)-out2bis(:,2)))

for i=1:1:deg
R_2(i)=sum(abs(out1_corr(i,:)-out2bis(:,2)'));
percentage(i)=(R_2(i))/total_dif_withoutcorr*100;

end

figure(7)
subplot(2,1,1)
plot(1:1:deg,R_2,'.g')
hold on
plot(0.5,total_dif_withoutcorr,'or')
subplot(2,1,2)
plot(1:1:deg,percentage,'.b')


save data_uni_ada.mat
return



