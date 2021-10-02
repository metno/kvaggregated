#! /bin/bash
export DOCKER_BUILDKIT=1

kvuser=kvalobs
kvuserid=5010
mode=test
target=kvaggregated
tag=latest
os=focal
#os=bionic
registry="registry.met.no/obs/kvalobs/kvbuild"
nocache=

#
#docker build --build-arg REGISTRY=registry.met.no/obs/kvalobs/kvbuild/staging/ -t registry.met.no/obs/kvalobs/kvbuild/staging/kvaggregated .    
#docker push registry.met.no/obs/kvalobs/kvbuild/staging/kvaggregated
#
use() {

  usage="\
Usage: $0 [--help] [--staging|--prod|--test] [--tag tag] [--no-cache]

This script build a kvagregated container. 

If --staging or --prod is given it is copied to the 
container registry at $registry. 
If --test, the default, is used it will not be copied 
to the registry.


Options:
  --help        display this help and exit.
  --tag tagname tag the image with the name tagname, default latest.
  --staging     build and push to staging.
  --prod        build and push to prod.
  --test        only build, default
  --no-cache    Do not use the docker build cache.
"
echo -e "$usage\n\n"

}

while test $# -ne 0; do
  case $1 in
    --tag) tag=$2;;
    --help) 
      use
      exit 0;;
    --staging) mode=staging;;
    --prod) prod=prod;;
    --test) test=test;;
    --no-cache) nocache="--no-cache";;
    -*) use
      echo "Invalid option $1"
      exit 1;;  
    *) targets="$targets $1";;
  esac
  shift
done


echo "tag: $tag"
echo "mode: $mode"
echo "os: $os"



echo "Build mode: $mode"

if [ $mode = test ]; then 
  registry=""
else 
  registry="$registry/$mode/"
fi

echo "registry: $registry"

docker build $nocache --build-arg BASE_IMAGE_TAG=$tag --build-arg REGISTRY=${registry} --build-arg "kvuser=$kvuser" --build-arg "kvuserid=$kvuserid" -f Dockerfile --tag ${target}:${tag} .
  

if [ $mode != test ]; then 
  docker tag ${target}:${tag} ${registry}${target}:${tag}
  docker push ${registry}${target}:${tag}
fi


