#!/bin/bash

# WebServ CGI Valgrind Testing Script
# This script helps run valgrind with proper suppressions for Python CGI testing

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Default values
WEBSERV_BINARY="./Webserv"
CONFIG_FILE="simple.conf"
SUPPRESSION_FILE="python_minimal.supp"
VALGRIND_LOG="valgrind_cgi.log"

# Function to print usage
usage() {
    echo "Usage: $0 [OPTIONS]"
    echo "Options:"
    echo "  -b, --binary <path>     Path to Webserv binary (default: $WEBSERV_BINARY)"
    echo "  -c, --config <path>     Path to config file (default: $CONFIG_FILE)"
    echo "  -s, --supp <path>       Path to suppression file (default: $SUPPRESSION_FILE)"
    echo "  -l, --log <path>        Path to valgrind log file (default: $VALGRIND_LOG)"
    echo "  -h, --help              Show this help message"
    echo ""
    echo "Example:"
    echo "  $0 -b ./Webserv -c simple.conf"
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -b|--binary)
            WEBSERV_BINARY="$2"
            shift 2
            ;;
        -c|--config)
            CONFIG_FILE="$2"
            shift 2
            ;;
        -s|--supp)
            SUPPRESSION_FILE="$2"
            shift 2
            ;;
        -l|--log)
            VALGRIND_LOG="$2"
            shift 2
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            usage
            exit 1
            ;;
    esac
done

# Check if binary exists
if [ ! -f "$WEBSERV_BINARY" ]; then
    echo -e "${RED}Error: Webserv binary '$WEBSERV_BINARY' not found!${NC}"
    echo -e "${YELLOW}Hint: Run 'make' first or specify correct path with -b option${NC}"
    exit 1
fi

# Check if config file exists
if [ ! -f "$CONFIG_FILE" ]; then
    echo -e "${RED}Error: Config file '$CONFIG_FILE' not found!${NC}"
    exit 1
fi

# Check if suppression file exists
if [ ! -f "$SUPPRESSION_FILE" ]; then
    echo -e "${YELLOW}Warning: Suppression file '$SUPPRESSION_FILE' not found!${NC}"
    echo -e "${YELLOW}Running valgrind without suppressions...${NC}"
    SUPPRESSION_ARG=""
else
    SUPPRESSION_ARG="--suppressions=$SUPPRESSION_FILE"
fi

echo -e "${GREEN}Starting Webserv CGI leak testing...${NC}"
echo -e "${YELLOW}Binary: $WEBSERV_BINARY${NC}"
echo -e "${YELLOW}Config: $CONFIG_FILE${NC}"
echo -e "${YELLOW}Suppressions: $SUPPRESSION_FILE${NC}"
echo -e "${YELLOW}Log file: $VALGRIND_LOG${NC}"
echo ""

# Valgrind command with comprehensive options
valgrind \
    --tool=memcheck \
    --leak-check=full \
    --show-leak-kinds=all \
    --track-origins=yes \
    --verbose \
    --log-file="$VALGRIND_LOG" \
    $SUPPRESSION_ARG \
    --child-silent-after-fork=yes \
    --trace-children=yes \
    --track-fds=yes \
    "$WEBSERV_BINARY" "$CONFIG_FILE"

echo -e "${GREEN}Valgrind testing completed!${NC}"
echo -e "${YELLOW}Check '$VALGRIND_LOG' for detailed results${NC}"

# Show summary of leaks
if [ -f "$VALGRIND_LOG" ]; then
    echo ""
    echo -e "${GREEN}=== LEAK SUMMARY ===${NC}"
    grep -E "(definitely lost|indirectly lost|possibly lost|still reachable)" "$VALGRIND_LOG" | tail -4

    echo ""
    echo -e "${GREEN}=== ERROR SUMMARY ===${NC}"
    grep -E "ERROR SUMMARY" "$VALGRIND_LOG" | tail -1
fi