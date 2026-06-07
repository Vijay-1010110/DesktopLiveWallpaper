from PIL import Image
import os

def remove_bg(img_path, out_path, bg_color):
    img = Image.open(img_path).convert("RGBA")
    datas = img.getdata()
    newData = []
    
    # We will do a simple distance check to remove background
    for item in datas:
        # Distance to background color
        dist = ((item[0] - bg_color[0])**2 + (item[1] - bg_color[1])**2 + (item[2] - bg_color[2])**2)**0.5
        if dist < 100: # Threshold
            newData.append((255, 255, 255, 0))
        else:
            newData.append(item)
            
    img.putdata(newData)
    
    # Crop to bounding box
    bbox = img.getbbox()
    if bbox:
        img = img.crop(bbox)
        
    # Resize to something reasonable for a particle (e.g. 64x64 max)
    img.thumbnail((64, 64), Image.LANCZOS)
    img.save(out_path, "PNG")

os.makedirs("d:/VS 22 projects/WeatherLIverWallpaper/WeatherLIverWallpaper/assets", exist_ok=True)

# Spring (Green bg)
remove_bg(r"C:\Users\Vijay\.gemini\antigravity\brain\1fe67b75-11a5-4f1e-bd74-51db5e5ddc0e\leaf_spring_1780819588022.png", 
          "d:/VS 22 projects/WeatherLIverWallpaper/WeatherLIverWallpaper/assets/leaf_spring.png", (0, 255, 0))

# Summer (Magenta bg)
remove_bg(r"C:\Users\Vijay\.gemini\antigravity\brain\1fe67b75-11a5-4f1e-bd74-51db5e5ddc0e\leaf_summer_1780819607185.png", 
          "d:/VS 22 projects/WeatherLIverWallpaper/WeatherLIverWallpaper/assets/leaf_summer.png", (255, 0, 255))

# Autumn (Magenta bg)
remove_bg(r"C:\Users\Vijay\.gemini\antigravity\brain\1fe67b75-11a5-4f1e-bd74-51db5e5ddc0e\leaf_autumn_1780819626032.png", 
          "d:/VS 22 projects/WeatherLIverWallpaper/WeatherLIverWallpaper/assets/leaf_autumn.png", (255, 0, 255))

# Winter (Magenta bg)
remove_bg(r"C:\Users\Vijay\.gemini\antigravity\brain\1fe67b75-11a5-4f1e-bd74-51db5e5ddc0e\snow_flake_1780819644126.png", 
          "d:/VS 22 projects/WeatherLIverWallpaper/WeatherLIverWallpaper/assets/snow_flake.png", (255, 0, 255))

print("Assets processed and saved!")
