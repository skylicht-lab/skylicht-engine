import os
import sys
import argparse
from ftplib import FTP, error_perm

def upload_file(ftp, local_path, remote_path):
    """Uploads a single file to the FTP server."""
    print(f"Uploading: {local_path} -> {remote_path}")
    with open(local_path, 'rb') as f:
        ftp.storbinary(f'STOR {remote_path}', f)

def ensure_remote_dir(ftp, remote_dir):
    """Ensures that the remote directory exists, creating parent directories if necessary."""
    parts = remote_dir.split('/')
    current = ""
    for part in parts:
        if not part:
            continue
        current = f"{current}/{part}" if current else part
        try:
            ftp.mkd(current)
        except error_perm as e:
            if not str(e).startswith('550'):
                raise

def upload_directory(ftp, local_dir, remote_dir):
    """Recursively uploads a directory to the FTP server."""
    print(f"Uploading directory: {local_dir} -> {remote_dir}")
    
    # Ensure the remote directory exists
    ensure_remote_dir(ftp, remote_dir)

    # Iterate through local directory items
    for item in os.listdir(local_dir):
        local_path = os.path.join(local_dir, item)
        remote_path = f"{remote_dir}/{item}".replace('\\', '/')

        if os.path.isfile(local_path):
            upload_file(ftp, local_path, remote_path)
        elif os.path.isdir(local_path):
            upload_directory(ftp, local_path, remote_path)

def main():
    parser = argparse.ArgumentParser(description='Upload a file or folder to an FTP server.')
    parser.add_argument('--host', help='FTP host address (or use FTP_HOST env var)')
    parser.add_argument('--user', help='FTP username (or use FTP_USER env var)')
    parser.add_argument('--password', help='FTP password (or use FTP_PASSWORD env var)')
    parser.add_argument('--src', required=True, help='Local file or folder path to upload')
    parser.add_argument('--dest', default='.', help='Remote destination directory (default: current)')
    parser.add_argument('--port', type=int, default=21, help='FTP port (default: 21)')

    args = parser.parse_args()

    host = args.host or os.environ.get('FTP_HOST')
    user = args.user or os.environ.get('FTP_USER')
    password = args.password or os.environ.get('FTP_PASSWORD')

    if not host or not user or not password:
        print("Error: FTP host, user, and password must be provided via arguments or environment variables.")
        sys.exit(1)

    if not os.path.exists(args.src):
        print(f"Error: Source path '{args.src}' does not exist.")
        sys.exit(1)

    try:
        print(f"Connecting to {host}:{args.port}...")
        ftp = FTP()
        ftp.connect(host, args.port)
        ftp.login(user, password)
        print("Successfully logged in.")

        remote_base = args.dest.rstrip('/')
        if remote_base and remote_base != '.':
            ensure_remote_dir(ftp, remote_base)

        if os.path.isfile(args.src):
            # If source is a file, upload it to the destination directory
            filename = os.path.basename(args.src)
            remote_path = f"{remote_base}/{filename}" if remote_base and remote_base != '.' else filename
            upload_file(ftp, args.src, remote_path)
        elif os.path.isdir(args.src):
            # If source is a directory, upload its contents recursively
            dirname = os.path.basename(os.path.normpath(args.src))
            remote_path = f"{remote_base}/{dirname}" if remote_base else dirname
            upload_directory(ftp, args.src, remote_path)

        ftp.quit()
        print("Upload complete. Connection closed.")

    except Exception as e:
        print(f"An error occurred: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()
