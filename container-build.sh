#! /bin/bash
export DOCKER_BUILDKIT=1

kvuser=kvalobs
kvuserid=5010
mode="test"
target=kvaggregated
tag=
kvcpp_tag=latest
default_os=noble
os=noble
build="true"
push="true"
#os=bionic
registry="registry.met.no/met/obsklim/bakkeobservasjoner/data-og-kvalitet/kvalobs/kvbuild"
nocache=
BUILDDATE=$(date +'%Y%m%d')
VERSION="$(./version.sh)"
tag="$VERSION"
KV_BUILD_DATE=${KV_BUILD_DATE:-}
tags=""
tag_counter=0

if [ -n "${KV_BUILD_DATE}" ]; then
  BUILDDATE=$KV_BUILD_DATE
fi


use() {

  usage="\
Usage: $0 [--help] [options] 

This script build a kvagregated container. 

If --staging or --prod is given it is copied to the 
container registry at $registry. 
If --test, the default, is used it will not be copied 
to the registry.


Options:
  --help        display this help and exit.
  --tag tagname tag the image with the name tagname, default latest.
  --tag-and-latest tagname 
                tag the image with tagname and also create latest tag.
  --tag-with-build-date
                Creates three tags: ${VERSION}, latest and a ${VERSION}-${BUILDDATE}.
                If the enviroment variable KV_BUILD_DATE is set use
                this as the build date. Format KV_BUILD_DATE YYYYMMDD. 
  --staging     build and push to staging.
  --prod        build and push to prod.
  --test        only build, default
  --kvcpp-tag tag  Use this kvcpp-runtime/kvcpp-dev as base image. Default: $kvcpp_tag
  --no-cache    Do not use the docker build cache.
  --build-only  Stop after building.
  --push-only   Only push a previous build to registry. Must use the same flags as when builing.
  --print-version-tag
                Print the version and build date on the form version-YYYYMMDD.

  The following opptions is mutally exclusive: --tag, --tag-and-latest, --tag-with-build-date
  The following options is mutally exclusive: --build-only, --push-only
  The following options is mutally exclusive: --staging, --prod, --test
"
echo -e "$usage\n\n"

}

while test $# -ne 0; do
  case $1 in
    --tag) 
        tag=$2
        tag_counter=$((tag_counter + 1))
        shift
        ;;
    --help) 
      use
      exit 0;;
    --staging) mode="staging";;
    --prod) mode="prod";;
    --test) mode="test";;
    --tag-and-latest) 
        tag="$2"
        tags="latest"
        tag_counter=$((tag_counter + 1))
        shift
        ;;
    --tag-with-build-date)
        tags="latest $VERSION-$BUILDDATE"
        tag_counter=$((tag_counter + 1))
        ;;
    --kvcpp-tag) 
        kvcpp_tag="$2"
        shift 
        ;;
    --no-cache) nocache="--no-cache";;
    --build-only) push="false";;
    --push-only) build="false";;
    --print-version-tag)
        echo "$VERSION-$BUILDDATE"
        exit 0;;
    -*) use
        echo "Invalid option $1"
        exit 1;;  
      *) targets="$targets $1";;
  esac
  shift
done

if [ $tag_counter -gt 1 ]; then
  echo "Only one of --tag, --tag-and-latest or --tag-with-build-date can be used"
  exit 1
fi

echo "VERSION: $VERSION"
echo "mode: $mode"
echo "os: $os"
echo "Build mode: $mode"
echo "targets: $targets"
echo "build: $build"
echo "push: $push"
echo "tag: $tag"
echo "kvcpp_tag: $kvcpp_tag"


if [ "$mode" = test ]; then 
  registry="$os/"
  kvuserid=$(id -u)
elif [ "$os" = "$default_os" ]; then
  registry="$registry/$mode/"
else
  registry="$registry/$mode-$os/"
fi

echo "registry: $registry"

if [ "$build" = "true" ]; then
  docker build $nocache --build-arg BASE_IMAGE_TAG="${kvcpp_tag}" --build-arg REGISTRY=${registry} \
    --build-arg "kvuser=$kvuser" --build-arg "kvuserid=$kvuserid" \
    -f Dockerfile --tag "${registry}${target}:${tag}" .
  for tagname in $tags; do
    echo "Tagging: ${registry}${target}:$tagname"
    docker tag "${registry}${target}:$tag" "${registry}${target}:$tagname"
  done
fi

if [ "$mode" != "test" ] && [ "$push" = "true" ]; then 
  # Before pushing add the tag to the list of tags
  tags="$tag $tags"
  for tagname in $tags; do
      echo "Pushing: ${registry}${target}:$tagname"
      docker push "${registry}${target}:$tagname"
  done
fi
