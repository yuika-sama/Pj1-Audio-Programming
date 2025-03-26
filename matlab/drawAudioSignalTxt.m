% Đọc dữ liệu từ file
data = load('guitar_signal.txt');

% Tạo trục thời gian giả định (số mẫu)
N = length(data);
t = 1:N;  % Nếu không có cột thời gian, sử dụng index làm trục hoành

% Vẽ đồ thị tín hiệu
figure;
plot(t, data, 'b-', 'LineWidth', 1.5); % Vẽ đường màu xanh, độ dày 1.5
grid on;
xlabel('Mẫu (Index)');
ylabel('Biên độ');
title('Biểu đồ tín hiệu âm thanh');
legend('Tín hiệu');
