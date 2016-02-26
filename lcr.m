% reading the datafile.
X1 = csvread('1.csv');

% abstracting the real and imaginary parts of the dataset.
X1_rl = X1(:,1);
X1_im = X1(:,2);

% The test bench frequency input.
z1 = linspace(1000,5000,4001);
z2 = linspace(5001,9981,250);

z = [z1,z2]';

%find max element of data(to find parallel resonance frequency).
max_index = find(X1_rl==max(X1_rl),1);
f_p = z(max_index);
w_p = f_p*2*pi;
R_max = max(X1_rl);

%bandwidth ~(3dbm) calculation.
%f1 and f2 map to (R_max)
%                 -------
%                 sqrt(2)

R_bw = R_max/(sqrt(2));
min1 = R_max;
in_min1 = 1;
min2 = R_max;
in_min2 = 1;

for i = 1:max_index
	if(abs(R_bw-X1_rl(i))<min1)
		min1 = R_bw-X1_rl(i);
		in_min1 = i;
	end
end

for i = max_index:length(z)
	if(abs(X1_rl(i)-R_bw)<min2)
		min2 = X1_rl(i)-R_bw;
		in_min2 = i;
	end
end

f_1 = z(in_min1);
w_1 = 2*pi*f_1;
f_2 = z(in_min2);
w_2 = 2*pi*f_2;

%bandwith
w_bw = w_2-w_1;

% Q-factor
Q = w_p/w_bw;


%			     R1
%			----^^^^----
%			|	   |	
% 	R0	C0	|    C1	   |	
%   ---^^^^-----||------|----||----|-------
%			|	   |
%			|    L1	   |	
%			---()()()---

%R0
R0 = X1_rl(length(X1_rl));

I0 = X1_im(length(X1_im));
I0 = -1*I0;
wC0 = 1/I0;

%C0
temp = z(length(z));
temp = 2*pi*temp;
C0 = wC0/temp;

%R1
R1 = R_max - R0;

%L1
L1 = R1/(w_p*Q);

%C1
C1 = 1/(L1*(w_p^2));

final = [R0,C0,R1,L1,C1];

