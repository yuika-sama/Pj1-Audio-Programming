### Lời mở đầu

- Đây là bài tập lớn của nhóm **SNDPROGSP25PRJ007**  môn Lập trình âm thanh
- **Giảng viên**: Phạm Văn Sự 
- Đây là bài tập lớn thứ 1/5 của bộ môn Lập trình âm thanh
- Thành viên nhóm:
	- Nguyễn Đức Anh - B22DCPT009
	- Nguyễn Trọng Chức - B22DCPT033
	- Văn Thiên Phúc - B22DCPT209
	- Phạm Hà Thắng - B22DCPT261
- Các chương trình sử dụng ngôn ngữ C++ và các thư viện sẵn có khi cài đặt phần mềm **Embarcadero Dev C++**, đồng thời có sử dụng phần mềm MatLab để biểu diễn tín hiệu


###Tính năng

- Đọc, xử lý và xuất tín hiệu âm thanh từ file định dạng .wav hoặc .mp3
- Phát âm thanh nhận từ file .wav hoặc .mp3
- Vẽ tín hiệu nhận từ file .wav(bằng code c++)
- Vẽ tín hiệu nhận từ file .wav(bằng ứng dụng matlab)


### Giới thiệu

- Repository này chứa các file source code với vai trò như sau
	- **main.cpp**: Gọi namespace Interface để xử lý dữ liệu truyền vào đối tượng AudioSignal
		- **AudioSignal.cpp**: Chứa thông tin class AudioSignal, với vai trò đọc, ghi, xử lý tín hiệu âm thanh được truyền vào
		- **Interface.cpp**: Chứa thông tin namespace Interface, với vai trò xử lý giao diện người dùng trên giao diện cmd
	- **ReadMP3.cpp**: Chương trình độc lập xử lý đọc và ghi tín hiệu MP3
	- **ReadWAV.cpp**: Chương trình độc lập xử lý đọc và ghi tín hiệu WAV
	- **PlaySound.cpp**: Chương trình xử lý phát file Audio với định dạng .mp3 hoặc .wav
	- **DrawSound.cpp**: Chương trình xử lý vẽ tín hiệu với C++
	- **Folder matlab**: Chứa các đoạn mã đọc và in ra tín hiệu âm thanh trên ứng dụng MatLab
	- **Folder resources/sounds**: Chứa tài nguyên âm thanh, **cần phải lưu các file âm thanh vào thư mục này để chương trình tìm thấy**
	- **Folder output**: Chứa đầu ra các file .txt là kết quả xuất tín hiệu


### Cài đặt

- Yêu cầu cài đặt **Embarcadero Dev C++** để chạy chương trình
- **Tiền xử lý:** Trước khi build chương trình, cần xác định linker và hệ thống folder cho chương trình
	- **Linker:**
		- Cách thêm Linker cho từng chương trình:
			- Tại giao diện người dùng, chọn **Tools -> Compiler Options**
			- Tại dòng **Add the following commands when calling the linkers**, thêm các linker phù hợp(sẽ được liệt kê sau)
			- Khi thêm linker, không khuyến khích xoá linker sẵn có là **-static-libgcc**
			- Khi build các chương trình khác nhau, cần đảm bảo linker phù hợp(VD: Khi build chương trình** ReadMp3.cpp**, sau khi thêm linker **-lwinmm** mà thêm linker **-mwindows** thì sẽ bị lỗi không chạy)
		- Các linker cần có cho từng file:
|			- **main.cpp**: không cần
|			- **ReadMp3.cpp, ReadWav.cpp, PlaySound.cpp**: -lwinmm
|			- **DrawSound.cpp**: -mwindows
	- **Hệ thống thư mục:** Cần triển khai hệ thông thư mục theo biểu đồ sau:
		```
		Folder nguồn: 
		│ "code files.cpp"
		├── resources
		│   ├── sounds
		│
		└── output
		│
		└── matlab
		```
- **Build:** Sau khi thêm linker phù hợp cho File, nhấn F9(Build) để kiểm tra lỗi xuất hiện. Nếu không có, nhấn F12 để chạy chương trình
