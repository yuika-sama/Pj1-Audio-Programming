[y, Fs] = audioread('guitar.mp3'); % Đọc file WAV/MP3
t = (0:length(y)-1) / Fs; % Tạo trục thời gian

plot(t, y); % Vẽ tín hiệu
xlabel('Thời gian (s)');
ylabel('Biên độ');
title('Dạng sóng tín hiệu âm thanh');
grid on;
