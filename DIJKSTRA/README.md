# Lập trình song song

> Hệ thống sẽ bao gồm 1 máy master và 3 máy slaves

## Các bước cài đặt
### Bước 1: Cài đặt và thiết lập ssh
- Cập nhật các máy trong hệ thống bằng cách thực hiện các câu lệnh
```ssh
sudo apt update
sudo apt upgrade
```
- Sau khi chạy xong, các máy trong  hệ thống đã được cập nhật
- Tiếp theo ta cần phải lấy được địa chỉ IP của mỗi máy
```ssh
sudo apt install tool-nets
```
- Sau khi cài đặt, chúng ta thực hiện lấy IP của máy bằng cách thực hiện câu lệnh
```ssh
ifconfig
```
- Thực hiện cài đặt ssh server và ssh client trên các máy
```ssh
sudo apt install openssh-server
sudo apt install openssh-client
```
- Sau khi cài đăt ssh, ta thực hiện sinh khóa rsa bằng cách thực hiện câu lệnh

```ssh
ssh-keygen -t rsa
```

- Nhập folder muốn lưu trữ khóa, nếu muốn để mặc định thì ấn Enter
- Nhập mật khẩu khóa (không bắt buộc)
- Sao chép các file id_rsa_xxx.pub của các slave vào folder ~/.ssh/ vào máy master
- Sao chép file id_rsa_master.pub của máy master vào folder ~/.ssh/ vào các máy slaves
- Sau khi đã sao chép vào, mỗi máy slave sẽ có 3 files và máy master sẽ có 5 files
- Trong máy master, thực hiện các câu lệnh

```ssh
cat /home/mpiuser/.ssh/id_rsa_slaves1.pub >> /home/mpiuser/.ssh/authorized_keys
cat /home/mpiuser/.ssh/id_rsa_slaves2.pub >> /home/mpiuser/.ssh/authorized_keys
cat /home/mpiuser/.ssh/id_rsa_slaves3.pub >> /home/mpiuser/.ssh/authorized_keys
```

- Đối với các máy slave, thực hiện câu lệnh
```ssh
cat /home/mpiuser/.ssh/id_rsa_master.pub >> /home/mpiuser/.ssh/authorized_keys
```

- Sử dụng trình soạn thảo văn bản bất kỳ (nano, gedit, vi, ...) để thực hiện thay đổi file config trong đường dẫn /etc/ssh/sshd_config
- Thêm 2 dòng sau vào trong file và lưu lại

```ssh
PubkeyAuthentication yes
RSAAuthentication yes
```

- Sau khi thực hiện, các máy restart lại ssh 

```ssh
service ssh restart
```

- Tiếp theo ta thực hiện ssh vào các máy
```ssh
ssh mpiuser@<ip_address>
```
- Ta nhập mật khẩu của user và mật khẩu khóa (nếu cần)

### Bước 2: Cài đặt NFS server trên máy master và NFS client trên các máy slaves (Network File System)
- Trên máy master, ta cài đặt NFS master
```ssh
sudo apt install nfs-kernel-server
```

- Tạo thư mục chung cho việc chia sẻ file
```
sudo mkdir -p /home/mpiuser/Desktop/sharedfolder
```

- Thay đổi quyền của folder dùng chung
```
sudo chown nobody:nogroup /home/mpiuser/Desktop/sharedfolder
sudo chmod 777 /home/mpiuser/Desktop/sharedfolder
```

- Sửa file /etc/exports, thêm 2 dòng sau
```
/home/mpiuser/Desktop/sharedfolder <slave1IP>(rw, sync, no_subtree_check)
/home/mpiuser/Desktop/sharedfolder <slave2IP>(rw, sync, no_subtree_check)
/home/mpiuser/Desktop/sharedfolder <slave3IP>(rw, sync, no_subtree_check)
```

- Sau đó exports thư mục dùng chung
```
sudo exportfs -a
```

- Restart NFS server
```
sudo systemctl restart nfs-kernel-server
```

- Kiểm tra firewall
```
sudo ufw status
```

- Trên máy slave, ta cài đặt NFS client 
```ssh
sudo apt install nfs-common
```

- Tạo thư mục 
```
sudo mkdir -p /home/mpiuser/Desktop/sharedfolder
```

- Mount shared folder (tại các slave) to sharedfolder (tại master)
```
sudo mount <serverIP>:/home/mpiuser/Desktop/sharedfolder /home/mpiuser/Desktop/sharedfolder
```

### Bước 3: Cài đặt OpenMPI
- Cài đặt gcc cho tất cả các máy
```
sudo apt install gcc
```

- Cài đặt một vài thư viện cần thiết
```
sudo apt install openmpi-bin openmpi-common libopenmpi-dev libgtk2.0-dev
```

- Download và giải nén openmpi cho các máy, truy cập open-mpi.org, download open-mpi, [openmpi-5.0.2.tar.gz](https://download.open-mpi.org/release/open-mpi/v5.0/openmpi-5.0.2.tar.gz), sau đó copy file này tới /home/mpiuser/Desktop, sau đó tới thư mục /home/mpiuser/Desktop và giải nén
```
tar -xvf /home/mpiuser/Desktop/<file_name>
```

- Truy cập /home/mpiuser/Desktop/openmpi-version, sau đó chạy 3 lệnh
```
./configure --prefix="/home/mpiuser/.openmpi"
make
sudo make install
```
- Export PATH
- Thêm 2 dòng sau vào cuối file .bashrc và lưu lại
```
sudo nano .bashrc
```
```
export PATH="/home/mpiuser/.openmpi/bin:$PATH"
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/home/mpiuser/.openmpi/lib"
```

- Test bằng cách chạy mpirun hoặc mpicc (Hệ thống biết có lệnh mpirun là được)
```
mpirun
```
### Bước 4: Chạy Test
- Tại máy master, sửa file /etc/hosts
```
127.0.0.1 localhost
<slaveIP1> slave1
<slaveIP2> slave2
<slaveIP3> slave3
```

- Tại máy slave, sửa file /etc/hosts
```
127.0.0.1 localhost
<masterIP> master
```

- Dịch file mpi
- Tại máy master, tải ví dụ từ dropbox [Link](https://www.youtube.com/redirect?event=video_description&redir_token=QUFFLUhqbTYtMXlGcHQ3NVRBRGdwTEp6RmRNb2lJUXhNd3xBQ3Jtc0ttU09LbkxldnJ0QklBNnlFOUpNM0FSdVRwV0lVY19QWUIwTG1DTXROQ25DY0V3UmZPaWZwYWNOZlZNYWdmMXRMWW5vcHJHaW1fMnVrLVJrNDVmSEdJaFk1NHdaWk5jeTBJc2o1NnIxSUJHNUdfc1BBSQ&q=https%3A%2F%2Fwww.dropbox.com%2Fsh%2Fmhwg5n9oajtme98%2FAAAAlCd5GQDRP9WdMe13Ydija%3Fdl%3D0%26preview%3Dmpi-prime.c&v=HLTm5-bVt7c), sau đó chạy
```
mpicc <mpi_file> -o ./outputfile
```
- Sau đó copy outputfile tới sharedfolder, các slave kiểm tra xem đã có file ở trên sharedfolder chưa

- Chạy cụm, máy master tới sharedfolder, sau đó chạy
```
mpirun --hostfile /etc/hosts -np 5 ./outputfile
```

- Tại máy slave, chạy lệnh (Kiểm tra xem có process tên là outputfile chưa)
```
top
```
