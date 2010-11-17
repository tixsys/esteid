#!/bin/sh

echo "{$(uuidgen | tr a-z A-Z)}"
