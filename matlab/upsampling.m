x = audioread('samplewav.wav');
x1 = x(10000:10100);
L = 2; % tăng gấp 2 lần
z = zeros(length(x1)*L, 1);
for i = 1:length(x1)
    z((i-1)*L + 1) = x1(i);
end
plot(z);