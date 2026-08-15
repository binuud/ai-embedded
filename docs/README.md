## ROS

Webcam streaming is becoming a problem on esp32. No Good cameras on it as well, atleast not available in India.
aliexpress is banned in India, becoming an issue to get components.
So I have decided to start using raspberry pi, cost is huge. 

Trying ROS on mac first, then shift to pi.

Pull docker image
```
docker pull --platform linux/amd64 osrf/ros:lyrical-desktop
```

Give a name to the container, so its easy to open multiple terminals to the same container.
Also, we can restart the same container, instead of restarting everything from beginning.
```
docker run --name lyrical-ros -d -it  --platform linux/amd64 -v ~/Code/dockerVols/ros:/workspace osrf/ros:lyrical-desktop 
```

Run following command inside docker container to update the packages
```
apt-get update
apt install ros-lyrical-turtlesim
echo "source /opt/ros/lyrical/setup.bash" >> ~/.bashrc
```

Start in daemon mode
```
docker run --name lyrical-ros  -p 5901:5901 -d -it -e DISPLAY=host.docker.internal:0.0 -e LIBGL_ALWAYS_INDIRECT=0 --platform linux/amd64 -v ~/Code/dockerVols/ros:/workspace osrf/ros:lyrical-desktop
```


```
docker run --name lyrical-ros -p 6080:80 -p 5901:5901  -p 6901:6901 -e VNC_PW=password123 -e DISPLAY=host.docker.internal:0.0  --name ros2-lyrical --security-opt seccomp=unconfined --shm-size=512m ghcr.io/tiryoh/ros2-desktop-vnc:lyrical
```

Working
```
docker run --name ros2-lyrical -p 6080:80 -p 5901:5901  -p 6901:6901 -e VNC_PW=password123 -e DISPLAY=host.docker.internal:0.0 --shm-size=512m ghcr.io/tiryoh/ros2-desktop-vnc:lyrical
```

Run below in the container, so the ros environment is present, each time we get the shell access
```
apt-get update
apt install ros-lyrical-turtlesim
echo "source /opt/ros/lyrical/setup.bash" >> ~/.bashrc
```

to change user
```
su ubuntu 
export DISPLAY=:1
```

Start stop the container
```
docker start ros2-lyrical
docker stop ros2-lyrical
```

Shell access
```
docker exec -it ros2-lyrical bash -i
```
The source command is important to setup the paths and other variables. Alternatively add the same in bash.rc in the container

if ros2 command does not work, source the setup file manually
```
source /opt/ros/lyrical/setup.bash
```


Run below commands on individual/different terminals to check ros2 signalling
```
ros2 run demo_nodes_cpp talker
```

```
ros2 run demo_nodes_cpp listener
```

For testing VNC conncetivity use this, then connect to port 5901 using vnc client on your desktop
```
docker run -d -p 5901:5901 -p 6901:6901 -e VNC_PW=password123 --platform linux/amd64 consol/ubuntu-xfce-vnc
```

For reference the above image uses ubuntu Ubuntu 24.04.4 LTS, noble


### For Beginers
[Follow Youtube Guide](https://www.youtube.com/watch?v=qWuudNxFGOQ)
