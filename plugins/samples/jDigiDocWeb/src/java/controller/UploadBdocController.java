/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package controller;

import ee.sk.digidoc.DataFile;
import ee.sk.digidoc.factory.DigiDocFactory;
import ee.sk.utils.ConfigManager;
import java.io.File;
import java.io.IOException;
import java.util.Iterator;
import java.util.List;
import javax.servlet.RequestDispatcher;
import javax.servlet.ServletContext;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import org.apache.commons.fileupload.FileItem;
import org.apache.commons.fileupload.disk.DiskFileItemFactory;
import org.apache.commons.fileupload.servlet.ServletFileUpload;

/**
 *
 * @author lauri
 */
public class UploadBdocController extends HttpServlet {
   
    /** 
     * Processes requests for both HTTP <code>GET</code> and <code>POST</code> methods.
     * @param request servlet request
     * @param response servlet response
     * @throws ServletException if a servlet-specific error occurs
     * @throws IOException if an I/O error occurs
     */
    protected void processRequest(HttpServletRequest request, HttpServletResponse response)
    throws ServletException, IOException {
        ServletContext sc = getServletContext();
        request.setAttribute("suva", "test");
        String rootPath=getServletContext().getRealPath("/").toString();
            ConfigManager.init(rootPath+ "WEB-INF/jdigidoc.cfg");

            try{
                ServletFileUpload servletFileUpload = new ServletFileUpload(new DiskFileItemFactory());
                if (ServletFileUpload.isMultipartContent(request)) {
                   List fileItemsList = servletFileUpload.parseRequest(request);
                    Iterator it = fileItemsList.iterator();
                    while (it.hasNext()) {
                        FileItem fileItem = (FileItem) it.next();
                        if (!fileItem.isFormField()) {
                            String itemName = fileItem.getName();
                            if (!"".equals(itemName)) {
                                File file = new File(rootPath + "docks\\" + itemName);
                                fileItem.write(file);
                                DigiDocFactory digFac = ConfigManager.instance().getDigiDocFactory();
                                request.setAttribute("bdoc",digFac.readSignedDoc(rootPath + "docks\\" + itemName));
                                RequestDispatcher rd = sc.getRequestDispatcher("/bdocControl.jsp");
                                rd.forward(request, response);
                                return;
                            }
                        }
                    }
                }
            }
            catch(Exception ex)
            {
                response.setContentType("text/plain");
                response.getWriter().write(ex.getMessage());
                return;
            }

    } 

    // <editor-fold defaultstate="collapsed" desc="HttpServlet methods. Click on the + sign on the left to edit the code.">
    /** 
     * Handles the HTTP <code>GET</code> method.
     * @param request servlet request
     * @param response servlet response
     * @throws ServletException if a servlet-specific error occurs
     * @throws IOException if an I/O error occurs
     */
    @Override
    protected void doGet(HttpServletRequest request, HttpServletResponse response)
    throws ServletException, IOException {
        processRequest(request, response);
    } 

    /** 
     * Handles the HTTP <code>POST</code> method.
     * @param request servlet request
     * @param response servlet response
     * @throws ServletException if a servlet-specific error occurs
     * @throws IOException if an I/O error occurs
     */
    @Override
    protected void doPost(HttpServletRequest request, HttpServletResponse response)
    throws ServletException, IOException {
        processRequest(request, response);
    }

    /** 
     * Returns a short description of the servlet.
     * @return a String containing servlet description
     */
    @Override
    public String getServletInfo() {
        return "Short description";
    }// </editor-fold>

}
