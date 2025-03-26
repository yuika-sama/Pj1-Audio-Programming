% Read audio files
x = audioread('samplewav.wav');
x1 = x(10000:10100);
y = audioread('samplelongwav.wav');
y1 = y(10000:10120);

% Get lengths of signals
x1_len = length(x1);
y1_len = length(y1);

% Pad the shorter signal with zeros to match lengths
if x1_len < y1_len
    x1(end+1:y1_len) = 0;
elseif x1_len > y1_len
    y1(end+1:x1_len) = 0;
end

% Mix the signals
z = x1 + y1;

% Plot the mixed signal
plot(z);
