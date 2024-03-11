#! /bin/bash
export DOCKER_BUILDKIT=1

kvuser=kvalobs
kvuserid=5010
mode="test"
target=kvaggregated
tag=latest
tag_and_latest=false
kvcpp_tag=latest
os=focal
#os=bionic
registry="registry.met.no/met/obsklim/bakkeobservasjoner/data-og-kvalitet/kvalobs/kvbuild"
nocache=
BUILDDATE=$(date +'%Y%m%d')
VERSION="$(./version.sh)"


use() {

  usage="\
Usage: $0 [--help] [--no-cache] [--staging|--prod|--test] [--kvcpp-tag] [--tag tag] [--tag-with-build-date] 

This script build a kvagregated container. 

If --staging or --prod is given it is copied to the 
container registry at $registry. 
If --test, the default, is used it will not be copied 
to the registry.


Options:
  --help        display this help and exit.
  --tag tagname tag the image with the name tagname, default latest.
  --tag-with-build-date 
                tag with version and build date on the form version-YYYYMMDD 
                and set latest.
  --staging     build and push to staging.
  --prod        build and push to prod.
  --test        only build, default
  --kvcpp-tag tag  Use this kvcpp-runtime/kvcpp-dev as base image. Default: $kvcpp_tag
  --no-cache    Do not use the docker build cache.
"
echo -e "$usage\n\n"

}

while test $# -ne 0; do
  case $1 in
    --tag) 
        tag=$2
        shift
        ;;
    --help) 
      use
      exit 0;;
    --staging) mode="staging";;
    --prod) mode="prod";;
    --test) mode="test";;
    --tag-with-build-date)
        tag="$VERSION-$BUILDDATE"
        tag_and_latest=true;;
    --kvcpp-tag) 
        kvcpp_tag="$2"
        shift 
        ;;
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
echo "kvcpp_tag: $kvcpp_tag"
echo "Build mode: $mode"

if [ $mode = test ]; then 
  registry=""
else 
  registry="$registry/$mode/"
fi

echo "registry: $registry"

docker build $nocache --build-arg BASE_IMAGE_TAG="${kvcpp_tag}" --build-arg REGISTRY=${registry} \
  --build-arg "kvuser=$kvuser" --build-arg "kvuserid=$kvuserid" \
  -f Dockerfile --tag "${target}:${tag}" .

if [ ${tag} != "latest" ]; then
  docker tag "${target}:$tag" "${target}:latest"
fi

if [ "$mode" != "test" ]; then 
  docker tag "${target}:${tag}" "${registry}${target}:${tag}"
  docker push "${registry}${target}:${tag}"
  if [ "$tag_and_latest" = "true" ] &&  [ "${tag}" != "latest" ]; then
    docker tag "${registry}${target}:${tag}" "${registry}${target}:latest"
    docker push "${registry}${target}:latest"
  fi
fi


