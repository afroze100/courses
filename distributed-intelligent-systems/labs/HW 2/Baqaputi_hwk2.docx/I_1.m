% IMPORTANT NOTE: INDEX 1 REPRESENTS T=0, HENCE INDEX 31,REPRESENTS T=30

clear;

delta_s_r = 0.0992;     % initialization known values
delta_s_l = 0.1103;
delta_theta = 0.209;
delta_s = 0.10475;
axle = 0.053;
k_r = 0.03;
k_l = 0.03;

theta = zeros(31,1);    % initialize values of the three position variables
x = zeros(31,1);
y = zeros(31,1);

P = zeros(3,3,31);      % position uncertainty matrix initialization


for index=2:31          
     
% start filling in the values for position variables
    
theta(index) = theta(index-1) + delta_theta;

delta_x = delta_s * cos(theta(index-1) + delta_theta/2);
delta_y = delta_s * sin(theta(index-1) + delta_theta/2);

x(index) = x(index-1) + delta_x;
y(index) = y(index-1) + delta_y;

% now the uncertainty values

F_p(1,1) = 1;
F_p(2,2) = 1;
F_p(3,3) = 1;
F_p(1,3) = -y(index);
F_p(2,3) = x(index);

delta_P(1,1) = (k_r * delta_s_r).^2;
delta_P(2,2) = (k_l * delta_s_l).^2;

delta_F(1,1) = (0.5*cos(theta(index-1)+delta_theta/2)) - (delta_y/(2*axle));
delta_F(1,2) = (0.5*cos(theta(index-1)+delta_theta/2)) + (delta_y/(2*axle));
delta_F(2,1) = (0.5*sin(theta(index-1)+delta_theta/2)) + (delta_x/(2*axle));
delta_F(2,2) = (0.5*sin(theta(index-1)+delta_theta/2)) - (delta_x/(2*axle));
delta_F(3,1) = 1/axle;
delta_F(3,2) = -1/axle;

P(:,:,index) = F_p*P(:,:,index-1)*F_p' + delta_F*delta_P*delta_F';
                   
end


fprintf('At T=30, x = %d, y = %d\n\n',x(31),y(31));
disp('The position uncertainty at T=30 is as follow:');
disp(P(:,:,31));