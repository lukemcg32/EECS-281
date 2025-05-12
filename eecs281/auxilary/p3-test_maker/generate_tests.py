import re
import os

# Function to ensure IPs are in range
def fix_ip(ip):
    parts = ip.split('.')
    return '.'.join([str(min(max(int(part), 0), 254)) for part in parts])

# Define files to process
files_to_fix = [
    "test-13-commands.txt", "test-13-reg.txt",
    "test-14-commands.txt", "test-14-reg.txt",
    "test-15-commands.txt", "test-15-reg.txt"
]

# Create a backup directory for original files
os.makedirs("fixed_files", exist_ok=True)

# Process each file
for filename in files_to_fix:
    with open(filename, "r") as file:
        content = file.readlines()
        
    new_content = []
    ip_mapping = {}
    for line in content:
        modified_line = line
        # Find all IPs in the line
        ips = re.findall(r'\b(?:\d{1,3}\.){3}\d{1,3}\b', line)
        for ip in ips:
            if ip not in ip_mapping:
                ip_mapping[ip] = fix_ip(ip)
            modified_line = modified_line.replace(ip, ip_mapping[ip])
        new_content.append(modified_line)
    
    # Write fixed content to new file
    with open(f"fixed_files/{filename}", "w") as fixed_file:
        fixed_file.writelines(new_content)

print("Fixed files saved in 'fixed_files' directory.")