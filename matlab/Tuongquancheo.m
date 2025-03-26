x = audioread('samplewav.wav');
y = audioread('samplelongwav.wav');
x1 = x(10000:10100);
y1 = y(10000:10120);
z = xcorr(x1, y1);
plot(z);