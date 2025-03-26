x = audioread('samplewav.wav');
x1 = x(10000:10100);
M = 2; % giảm xuống còn 1/2
z = x1(1:M:end);
plot(z);