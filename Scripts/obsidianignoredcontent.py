import boto3
import os
import zipfile
import sys
import argparse

from datetime import datetime

# Config
OBSIDIAN_DIR = os.path.abspath(os.path.join(os.getcwd(), '..'))
GITIGNORE_PATH = os.path.join(OBSIDIAN_DIR, '.gitignore')
ZIP_FILENAME = f'obsidian_ignored_assets_{datetime.now().strftime("%Y%m%d_%H%M%S")}.zip'
ASSET_SECTION_HEADER = '# Ignore Assets'
S3_BUCKET = ''
S3_KEY_PREFIX = ''

def parse_gitignore_assets_section():
    asset_paths = []
    bFound = False

    with open(GITIGNORE_PATH, 'r') as gitignore:
        for line in gitignore:
            line = line.strip()
            if not line:
                continue
            if line.startswith('#'):
                if line == ASSET_SECTION_HEADER:
                    bFound = True
                    continue
                elif bFound:
                    break

            if bFound and not line.startswith('#'):
                clean_path = os.path.normpath(os.path.join(OBSIDIAN_DIR, line))
                asset_paths.append(clean_path)

    return asset_paths

def zip_ignored_assets(asset_paths):
    with zipfile.ZipFile(ZIP_FILENAME, 'w', zipfile.ZIP_DEFLATED) as zipped_asset:
        for path in asset_paths:
            if os.path.exists(path):
                if os.path.isfile(path):
                    zipped_asset.write(path, arcname=path)
                else:
                    for root, _, files in os.walk(path):
                        for file in files:
                            file_path = os.path.join(root, file)
                            arcname = os.path.relpath(file_path, OBSIDIAN_DIR)
                            zipped_asset.write(file_path, arcname)
    print(f"Created zip file: {ZIP_FILENAME}")

def upload_to_s3(bucket_name, object_key):
    s3 = boto3.client('s3')
    print(f"Uploading {ZIP_FILENAME} to s3://{bucket_name, object_key}...")
    s3.upload_file(ZIP_FILENAME, bucket_name, object_key)
    print("Upload complete.")

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Upload/Download Ignored Content Script')
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument('-download', '-d', action="store_true", help='Download ignored assets from s3 storage.')
    group.add_argument('-upload', '-u', action='store_true', help='Upload ignored assets to s3 storage.')
    args = parser.parse_args()
    
    if args.download:
        print("Downloading ignored assets..")
        
        asset_paths = parse_gitignore_assets_section()
    
        if not asset_paths:
            print(f"No assets found under section '{ASSET_SECTION_HEADER}' in .gitignore.")
        else:
            print("Parsed ignored assets, starting download..")
            

    elif args.upload:
        print("Uploading ignored assets..")

        asset_paths = parse_gitignore_assets_section()
    
        if not asset_paths:
            print(f"No assets found under section '{ASSET_SECTION_HEADER}' in .gitignore.")
        else:
            print("Zipping ignored asset files:")
            for path in asset_paths:
                print(f" - {path}")
            zip_ignored_assets(asset_paths)
            s3_key = f'{S3_KEY_PREFIX}{S3_BUCKET}'
            upload_to_s3(S3_BUCKET, s3_key)
            os.remove(ZIP_FILENAME)

        
