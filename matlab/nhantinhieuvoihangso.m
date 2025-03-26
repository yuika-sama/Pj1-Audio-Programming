% Đọc các tệp âm thanh
x = audioread('samplewav.wav');
x1 = x(10000:10100);
y = audioread('samplelongwav.wav');
y1 = y(10000:10120);

% Nhân tín hiệu với hằng số (ví dụ: 2)
z = y1 * 2;

% Vẽ đồ thị tín hiệu sau khi nhân
plot(z);