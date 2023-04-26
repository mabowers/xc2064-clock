## Docker for XC2064 development

1. If you don't have the Docker Engine installed already, follow the Docker installation steps below. Note that this Docker image was built and tested in rootless mode.

2. To build the XC2064 Docker image: `./docker/create.sh`

3. To run the XC2064 Docker container: `./docker/shell.sh`

4. Once at the XC2064 Docker's shell, you can do `./dosbox/run.sh` to start DOSBox.

## Docker installation

The following installation steps are based on this guide from Docker:<br>
https://docs.docker.com/engine/install/ubuntu/

(Alternatively, you may be able to install using `curl -sSL https://get.docker.com/rootless | sh`, but I haven't tested this. Also be advised that piping curl directly to shell can be dangerous, and should only be done with trusted sources.)


### Set up Docker repository

1. Update the `apt` package index and install packages to allow `apt` to use a repository over HTTPS:

```
sudo apt-get update
sudo apt-get install \
    ca-certificates \
    curl \
    gnupg
```

2. Add Docker’s official GPG key:

```
sudo install -m 0755 -d /etc/apt/keyrings
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo gpg --dearmor -o /etc/apt/keyrings/docker.gpg
sudo chmod a+r /etc/apt/keyrings/docker.gpg
```

3. Use the following command to set up the repository:

```
echo \
  "deb [arch="$(dpkg --print-architecture)" signed-by=/etc/apt/keyrings/docker.gpg] https://download.docker.com/linux/ubuntu \
  "$(. /etc/os-release && echo "$VERSION_CODENAME")" stable" | \
  sudo tee /etc/apt/sources.list.d/docker.list > /dev/null
```

### Install Docker Engine

1. Update the apt package index:
```
sudo apt-get update
```

2. Install Docker Engine, containerd, and Docker Compose.
```
sudo apt-get install docker-ce docker-ce-cli containerd.io docker-buildx-plugin docker-compose-plugin
```

3. Verify that the Docker Engine installation is successful by running the hello-world image:
```
sudo docker run hello-world
```

### Enable rootless Docker

The following steps to enable rootless Docker are based on this guide:<br>
https://docs.docker.com/engine/security/rootless/

1. Disable the system-wide Docker daemon:
```
sudo systemctl disable --now docker.service docker.socket
```

2. Try running `dockerd-rootless-setuptool.sh install`. The first time you run, it will print out instructions like these below. Follow these steps:
```
[ERROR] Missing system requirements. Run the following commands to
[ERROR] install the requirements and run this tool again.

########## BEGIN ##########
sudo sh -eux <<EOF
# Install newuidmap & newgidmap binaries
apt-get install -y uidmap
EOF
########## END ##########
```

3. Run `dockerd-rootless-setuptool.sh install` again and it should complete successfully.

4. After installation, it will give the following instructions to enable the Docker service:
```
[INFO] Installed docker.service successfully.
[INFO] To control docker.service, run: `systemctl --user (start|stop|restart) docker.service`
[INFO] To run docker.service on system startup, run: `sudo loginctl enable-linger $USER`
```

5. After enabling rootless mode, running `docker run hello-world` should now work *without* `sudo`.
