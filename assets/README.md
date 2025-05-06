Mã số sinh viên: 24020060 Họ và tên: Đỗ Hoàng Đạt

Lớp môn học: INT2215-4

I.Thông tin về game:
    

    - Tên game : Tank battle 2P (Phòng thủ xe tăng 2 người)
    
    - Video minh họa game: [demogame_Tank_Battle_2P.mp4](https://drive.google.com/file/d/17XAsHPe_2x65SJkHB3gY3VOAsDxF_ela/view?usp=sharing)
    
    - Link GitHub: https://github.com/DoHoangDat-K69IIT6/Tank-battle-2P
    
    - Lối chơi, logic của game:
    

    - Nhấn nút PLAY để vào chơi
    

    - Người chơi 1: Sử dụng W/A/S/D để di chuyển, [SPACE] để bắn đạn thường, [E] để dùng đạn nâng cấp
    
    - Người chơi 2: Sử dụng các phím mũi tên để di chuyển, [/] để bắn đạn thường, [m] để dùng đạn nâng cấp
    

    - Vào được căn cứ, chiếm đóng căn cứ của đối phương để chiến thắng, game sẽ kết thúc khi có người chơi vào được căn cứ của đối phương
    
    - Đạn có thể phá được tường, cần 3 phát mới có thể phá được tường
    
    - Khi đạn được nâng cấp, đạn có công lực bằng 2 phát bắn, phá được tường và người chơi
    

II.Đồ họa, âm thanh:
    

    - Đồ họa: SDL2
    
    - Âm thanh: SDL_mixer
    
    - SDL2_ttf: xử lí phông chữ trong game
    

III.Cấu trúc của project game: 
    

    - main.cpp: Luồng điều khiển logic chính của game, chứa game loop
    

    - Class Game (Game.cpp, Game.h):
    

        - Lớp trung tâm quản lí toàn bộ game
    
        - Khởi tạo và dọn dẹp các thành phần (SDL_mixer, SDL_ttf, …)
    
        - Khởi tạo người chơi, map
    
        - Quản lí danh sách đạn (vector <*Bullet> bullets)
    
        - Quản lí các trạng thái game (GAME OVER, PLAYING, MENU, CREDIT, QUIT)
    
        - Quản lí tài nguyên (texture, fonts, sound)
    
        - Xử lí các vòng lặp game: render(), update(), handleEvents()
    
        - Render giao diện menu
    
        - Xử lí logic chính của game (va chạm, điều kiện thắng, sinh nâng cấp trên map)
    

  

    - Class Player (Player.cpp, Player.h):
    

        - Vị trí, tốc độ, trạng thái, hướng nhìn của người chơi (xe tăng)
    
        - Quản lí texture theo hướng của người chơi (theo spritesheet)
    
        - Xử lí logic di chuyển và va chạm với các vật thể
    
        - Xử lí hồi sinh ( respawn() )
    

  

    - Class Bullet (Bullet.cpp, Bullet.h):
    

        - Vị trí, tốc độ, trạng thái của viên đạn khi được bắn ra
    
        - Quản lí texture theo hướng của viên đạn khi được bắn ra
    
        - Xử lí logic khi va chạm với tường
    
        - Xử lí logic khi bắn đạn được cường hóa
    
        - Render viên đạn khi bắn
    

  

    - Class TextureManager (TextureManager.cpp, TextureManager.h):
    

        - Cung cấp hàm để tải texture để tải texture từ file ảnh, sử dụng SDL_image
    
        - Cung cấp hàm để render texture
    

  

    - Folder /assets:
    

        - Chứa hình ảnh tường, spritesheet, texture của các vật thể trong game
    
        - Folder /sound: Chứa hiệu ứng âm thanh, nhạc nền được sử dụng trong game
    

  

IV.Các chức năng đã cài được cho game:
    

    1. Hệ thống trạng thái game:
    

        - Quản lí các trạng thái khác nhau của game: MENU, PLAYING, CREDIT, QUIT, GAME OVER
    
        - Có thể chuyển đổi qua lại giữa các trạng thái, người dùng có thể dễ dàng thao tác
    

    2. Điều khiển 2 người chơi
    

        - Người chơi 1 sử dụng WASD để di chuyển, SPACE để bắn đạn thường, E để bắn đạn cường hóa
    
        - Người chơi 2 sử dụng phím mũi tên để di chuyển, / để bắn đạn thường, M để bắn đạn cường hóa
    
        - Texture của xe tăng thay đổi khi hướng di chuyển thay đổi
    

    3. Hệ thống bắn đạn
    

        - Bắn đạn theo hướng xe tăng đang nhìn
    
        - Giới hạn tốc độ bắn, tránh bắn liên tục
    
        - Ảnh hiển thị tương ứng với chiều bắn
    

    4. Hệ thống va chạm
    

        - Người chơi với tường: Người chơi không thể đi qua được tường
    
        - Người chơi này không đi qua được người chơi khác
    
        - Đạn với tường: 
    

        - Đạn bị hủy khi va chạm với tường loại 1(không phá được)
    
        - Đạn phá được tường phá được (loại 2, 3, 4 trong map.txt) sau nhiều lần, thay đổi texture ở mỗi trạng thái rồi biến mất khi hết máu (loại 2->3->4)
    
        - Đạn được cường hóa có thể phá tường trong vùng 3x3 xung quanh điểm đạn va chạm với tường
    

        - Đạn với người chơi: Người chơi bị bắn sẽ hồi sinh trở lại tại điểm xuất phát, đạn bị hủy
    

    5. Hệ thống tải bản đồ
    

        - Tải cấu hình của bản đồ từ file text
    
        - Render bản đồ với các loại tường khác nhau (Đất trống, tường các loại, mục tiêu)
    
        - Có thể reset bản đồ về trạng thái ban đầu khi bắt đầu game mới bằng cách copy src_map.txt sang map.txt, các thay đổi áp dụng lên file map.txt
    

    6. Cơ chế Buff đạn
    

        - Item buff hiển thị ngẫu nhiên trên các ô trống sau một khoảng thời gian nhất định
    
        - Buff tồn tại trên bản đồ trong thời gian giới hạn
    
        - Người chơi được trang bị cường hóa khi đi qua buff
    
        - Người chơi có thể phá tường trong phạm vi 3x3 trong phạm vi va chạm với tường, buff bị tiêu hao sau khi bắn
    

    7. Giao diện người dùng 
    

        - Hiển thị tiêu đề game, các nút bấm (PLAY, CREDITS, QUIT). Chuyển được các trạng thái game sau khi click chuột
    
        - Màn hình GAME OVER hiển thị người chiến thắng và hướng dẫn quay lại MENU
    
        - Màn hình CREDITS hiển thị thông tin người làm
    
        - Sử dụng font chữ tùy chỉnh để hiển thị văn bản
    

    8. Âm thanh
    

        - Phát nhạc nền trong quá trình chơi game (trong trạng thái PLAYING)
    
        - Phát hiệu ứng âm thanh cho các hành động: bắn đạn, xe tăng bị bắn trúng, người chơi chiến thắng, nhặt được buff
    

    9. Điều kiện thắng thua
    

        - Người chơi chiến thắng khi di chuyển vào ô đã được chỉ định sẵn
    
        - Chuyển sang trạng thái GAMEOVER khi có người chiến thắng
    

    10. Quản lí tài nguyên
    

        - Sử dụng TextureManager để quản lí việc load và render hiệu quả, tránh bị lặp code
    
        - Giải phóng bộ nhớ sau khi sử dụng
    

  

V.Các thông tin khác:
    

    Cách cài đặt:

    B1: Tải game trên github, giải nén

    B2: Tìm file TANK-BATTLE-2P.exe và bắt đầu chơi!

   