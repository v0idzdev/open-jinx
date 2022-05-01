# Jinx OS
Jinx is a basic operating system built from scratch.

## Why Did I Decide to Create an OS?
I decided to start this project to learn how to make an OS. Hence, this software is **not** intended to be used by end users - at least, not unless it becomes stable and featureful enough for that to be possible.

A small disclaimer, most of the code in this repository has been copied, or at least heavily influenced by, various tutorials. After all, I'm trying to learn the ropes as an operating system developer, and am a complete beginner to this area of software engineering.

## How Can I Use Jinx OS?
At the moment, Jinx is very primitive and has, essentially, no features. When Jinx becomes somewhat featureful, I will add download links for ISO files. Nevertheless, you can run Jinx on Linux in a VM (I will use **QEMU** as an example) like this:

#### Clone the repository
```bash
git clone https://github.com/matthewflegg/open-jinx.git
cd open-jinx                            
```

#### Then, install QEMU:
```bash
sudo apt install qemu  # Ubuntu, Debian, etc.
```
```bash
sudo dnf in qemu  # Fedora, RHEL, etc.
```
```bash
sudo pacman -S qemu  # Arch, Manjaro, etc.
```

#### Run Jinx OS
```bash
chmod +x scripts/debug.sh 
./scripts/debug.sh
```

## References/Tutorials I Followed
Below, I've listed all of the tutorials that I used to create Jinx OS. 

* **[Daedelus | Making an OS | YouTube](https://www.youtube.com/watch?v=MwPjvJ9ulSc)**
* **[Poncho | OS Dev S2 | YouTube](https://www.youtube.com/playlist?list=PLxN4E629pPnJxCQCLy7E0SQY_zuumOVyZ)**
* **[nanobyte | Building an OS | YouTube](https://www.youtube.com/watch?v=9t-SPC7Tczc&list=PLFjM7v6KGMpiH2G-kT781ByCNC_0pKpPN)**
