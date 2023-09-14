#!/bin/sh
# This file is intended to do a teardown of the containers running on your computer if you don't
# want it to be running anymore. But still want to be able to fast start the containers again.

# removing network and stopping all containers
docker-compose down

# removing containers
docker rm ekrone-node1
docker rm ekrone-node2
docker rm ekrone-node3
docker rm ekrone-miner1