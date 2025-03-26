% Đọc các tệp âm thanh
x = audioread('samplewav.wav');
x1 = x(10000:10100);
y = audioread('samplelongwav.wav');
y1 = y(10000:10120);

% Lấy độ dài của các tín hiệu
x1_len = length(x1);
y1_len = length(y1);

% Thêm số không để làm cho độ dài của 2 tín hiệu bằng nhau
if x1_len < y1_len
    x1(end+1:y1_len) = 0;
elseif x1_len > y1_len
    y1(end+1:x1_len) = 0;
end

% Nhân 2 tín hiệu
z = x1 .* y1;

% Vẽ đồ thị tín hiệu sau khi nhân
plot(z);