![Jinx OS Logo](https://i.imgur.com/UmDPsfq.png)

# Jinx OS
> Jinx OS is a retro themed command line operating system for **x86_64** architectures. 
> If you wish to install or contribute towards Jinx, refer to the **[guide](#installation)** below.

## Installation
> It is recommended to build Jinx in Docker. Consequently, you must ensure that the **docker
> engine** is installed on your system. Additional tools are not required.
> 
> Execute the following script within a **Bash** terminal:
> ```bash
> git clone https://github.com/matthewflegg/open-jinx.git && \
>   sudo service docker start && \
>   sudo docker build buildenv -t jinx-buildenv && \
>   chmod +x buildenv/run.sh && \
>   sudo ./buildenv/run.sh
> ```
>
> Within the **Docker container**, run in interactive mode, execute the following command:
> ```bash
> make build-x86_64
> ```
> Once Jinx OS has been built, you will be able to emulate it within a virtual machine. In
> order to do this, it is recommended to use **QEMU**.
>
> Execute the following command within a bash terminal after exiting the Docker container
> mentioned in the previous step:
> ```bash
> qemu-system-x86_64 -cdrom dist/x86_64/kernel.iso
> ```
  

## References
> Below I've listed all of the resources, i.e. tutorials and guides, that I used to create Jinx OS. Please attribute all credit to the
> individuals who created said content.
>
> * **[Daedelus | Making an OS | YouTube](https://www.youtube.com/watch?v=MwPjvJ9ulSc)**
> * **[Poncho | OS Dev S2 | YouTube](https://www.youtube.com/playlist?list=PLxN4E629pPnJxCQCLy7E0SQY_zuumOVyZ)**
> * **[nanobyte | Building an OS | YouTube](https://www.youtube.com/watch?v=9t-SPC7Tczc&list=PLFjM7v6KGMpiH2G-kT781ByCNC_0pKpPN)**
