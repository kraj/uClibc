#! /bin/sh

${REAL_CC-gcc} $*

args=""
add_next=0

for i in $*
do
  if [ $add_next -eq 1 ] ; then
    [ "x$args" = "x" ] && args="$i" || args="$args $i"
	add_next=0
	continue
  fi
  case "/$i" in
  /-c) add_next=1 ;;
  /-D*|/-I*|/-inc) [ "x$args" = "x" ] && args="$i" || args="$args $i" ;;
  /-dumpversion|/--print*) ${CC-gcc} $i ; exit 0 ;;
  *) ;;
  esac
done

cppcheck $args
