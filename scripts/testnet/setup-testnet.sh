#!/bin/sh

# before running this shell script ensure you have docker and docker compose setup on your computer.
# as well as make sure that docker is running otherwise you will get a lot of errors.

# build the docker image for the ekrone daemon, miner and ekrwallet etc 
# (general build used for many purposes)

# uncomment the line below if you _need_ to compile it locally if you made some changes that is
# not pushed to master 

#docker build -t ekrone/ekrone-testnet -f Dockerfile.test .

# pull latest testnet image (uncomment this if you want to build yourself)
docker pull ghcr.io/ekrone/ekrone-testnet:latest

# start the testnet script
./start-testnet.sh 