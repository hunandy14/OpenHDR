## OpenHDR
#### 參考論文：  
細節與對比強化之高動態範圍影像顯示方法  
High Dynamic Range Image Display with Detail and Contrast Enhancement  

#### 使用函式庫：
- [rgbe](http://www.graphics.cornell.edu/~bjw/rgbe/)
- [OpenBMP](https://github.com/hunandy14/OpenBMP)

### 流程
1. 讀取 rgbe 轉換為 rgb
2. 轉換彩色模型至 Yxy
3. 對亮度通道做色調映射(tone mapping)
4. 色彩模型轉回 rgb
5. 進行 gama 校正 (微軟 Win10 設置為 2.2)
6. 完成 global tone mapping

### 測試結果
![](https://raw.githubusercontent.com/hunandy14/OpenHDR/master/OpenHDR/Snipaste_2018-06-05_01-19-08.png)

測試主機

- i7 4770k 預設時脈
- ddr3-2400-16G