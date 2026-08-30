#!/bin/bash

# Ensure the script stops immediately if an individual command fails
set -e

# Help instructions helper function
show_usage() {
    echo "Usage: $0 {up|down|build|migrate|logs}"
    echo "Options for logs:"
    echo "  $0 logs       - Show recent container logs"
    echo "  $0 logs -f    - Stream/follow container logs in real-time"
    exit 1
}

# Check if an argument was passed
if [ -z "$1" ]; then
    show_usage
fi

case "$1" in
  up)
    echo "🚀 Starting development servers in the background..."
    docker compose up -d
    echo "✨ Containers are running. Use '$0 logs -f' to view output."
    ;;
  down)
    echo "🛑 Stopping and removing project containers..."
    docker compose down 
    ;;
  build)
    echo "🔄 Rebuilding containers completely from scratch (clearing cache)..."
    docker compose build --no-cache
    ;;
  migrate)
    echo "📦 Running Django database migrations..."
    docker compose exec backend python manage.py migrate
    ;;
  makemigrations)
    echo "📦 Running Django database migrations..."
    docker compose exec backend python manage.py makemigrations
    ;;
  shell)
    echo "📦 Running Django database migrations..."
    docker compose exec backend python manage.py shell
    ;;
  logs)
    # Check if user passed the streaming '-f' flag as the second argument
    if [ "$2" = "-f" ]; then
        echo "📺 Streaming real-time telemetry and server logs (Ctrl+C to exit)..."
        docker compose logs -f
    else
        echo "📄 Displaying recent container logs..."
        docker compose logs
    fi
    ;;
  *)
    show_usage
    ;;
esac
